#ifndef __CLIENTPROFILEMANAGERNEW_H__
#define __CLIENTPROFILEMANAGERNEW_H__


#include "RomOrderNew.h"
#include "RomMapN.h"
#include "ROMTypedef.h"
#include "romvaluedefs.h"
#include "Connection.h"
#include <map>

typedef struct LIMIT
{
    double BuyingPower;
    double MaxDollars;
    long MaxShares;
    long MaxTotalShares;
    int MaxOrders;
    long NumShares;
    int NumOrders;
    bool exempted;
    double Balance[ROMVALUE::SIDENUM];

    struct LIMIT& operator=(const struct LIMIT& obj)
    {
        BuyingPower = obj.BuyingPower;
        MaxDollars = obj.MaxDollars;
        MaxShares = obj.MaxShares;
        MaxTotalShares = obj.MaxTotalShares;
        MaxOrders = obj.MaxOrders;
        NumShares = obj.NumShares;
        NumOrders = obj.NumOrders;
        exempted = obj.exempted;
        for (int i = 0; i < ROMVALUE::SIDENUM; i++)
        {
            Balance[i] = obj.Balance[i];
        }
        return *this;
    }

} tLimits;

typedef struct
{
    int id;
    std::string* name;
    bool exempted;
    tLimits limits[ROMVALUE::SECNUM];
} theLimits;
    
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
    CThreadLimits(const std::string& key, const tLimits& limits);
    CThreadLimits();
    ~CThreadLimits();

    bool ExceedLimit(const tBalanceUpdate& BUpdate, std::string& error);
    void UpdateBalance(const tBalanceUpdate& BUpdate);
    void GetLimits(tLimits& limits);
    void UpdateLimits(const tLimits& limits);
    const std::string& Key() { return  m_key; }
    void ImportBalance(const CThreadLimits& limits);
    void GetBalance(tLimits& limits);
    void SetKey(const std::string& key);
    void Exempt(bool action);
    bool Exempted();
    
private:
    tLimits m_limits;
    std::string m_key;
    pthread_rwlock_t m_lock;
};

class CBillingGrpLimits
{
public:
   
    CBillingGrpLimits();
    ~CBillingGrpLimits();
    
    int ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error);
    bool UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate);
    bool GetLimits(int, int secIndex, tLimits& limits);
    bool GetLimits(int, theLimits& limits);
    void ReloadFromDB(const int* billinggrpid);
    bool Exempt(int bgrpid, bool action);
    
private:
    bool LoadClrBillingGrpId(std::map<std::string, int>& billinggrptoid);
    pthread_spinlock_t m_lock;
    std::map<std::string, theLimits*> m_ClrToLimits;  
    std::map<int, theLimits*> m_BillingGrpIdToLimits;
};

class CGLimits
{
public:

    CGLimits();
    ~CGLimits();

    bool ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error);
    bool UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate);
    bool GetLimits(const std::string& key, tLimits& limits);
    bool AddLimits(const std::string& key, const tLimits& limits);
    bool UpdateLimits(const std::string& key, const tLimits& limits);
    void ReloadFromDB(const std::string* key = NULL);
    void SwapMap(CRomMap<CThreadLimits*>& map);
    void ImportBalance(const std::string& key, const CThreadLimits& limits);
    void CopyOutLimitsMap(CRomMap<tLimits*>& map);
    
    CGLimits& operator=(const CGLimits& obj);
    void CopyOutBalance(CGLimits* glimits);
    bool IsEmpty();
    bool Exempt(const std::string& key, bool action);
    bool Exempted(const std::string& key);
    bool Find(const std::string& key);
    
private:
    virtual bool ProcessInvalidKey(const std::string& key, std::string& error) = 0;
    virtual void ReloadAllLimits() = 0;
    virtual void ReloadLimits(const std::string* key) = 0;
    CRomMap<CThreadLimits*> m_limitsMap;
    pthread_rwlock_t m_lock;
};


class CClearingActLimits : public CGLimits
{
public:
    CClearingActLimits() {};
    ~CClearingActLimits() {};

    //void MakeSql(std::string& sql, const std::string* key = NULL);

private:
    virtual bool ProcessInvalidKey(const std::string& key, std::string& error)
    {
        return false;
    }
    virtual void ReloadAllLimits();
    virtual void ReloadLimits(const std::string* key);
    void MakeLimits(tLimits& limits, std::string& key, queue_t row);
};

class CUserFutureLimits : public CGLimits
{
public:
    CUserFutureLimits(const std::string& username) { m_username = username;};
    //~CUserFutureLimits() {};

    void ReLoad(const std::string* product);
    bool ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, bool exempted, std::string& error);
   
private:
    virtual bool ProcessInvalidKey(const std::string& product, std::string& error);
    virtual void ReloadAllLimits();
    virtual void ReloadLimits(const std::string* product);
    void MakeLimits(tLimits& limits, std::string& product, queue_t row);
    std::string m_username;
};

class CTraderData
{
public:

    void init();
    CTraderData(const std::string& trader) :
    m_Trader(trader),
    m_futureLimits(trader)
    {

        init();
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
    void SetMsgRateLimit(int value, unsigned short secIndex)
    {
        m_msgrateLimit[secIndex] = value;
    }

    std::string Trader() const
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
    int ResetDay()
    {
        return m_resetday;
    }
    time_t LastLogonTime()
    {
        return m_lastlogontime;
    }
    int msgrateLimit(unsigned short secIndex)
    {
        return m_msgrateLimit[secIndex];
    }

    int* msgrateLimit()
    {
        return &(m_msgrateLimit[0]);
    }

    bool IsSubTrader(const char* trader);
    void LoadSubTraders(std::string traders);
    void LoadSubTraders(tStringSet& traderSet);
    bool ExceedLimit(const tBalanceUpdate& theChange, std::string& error);
    bool ExceedFutureLimit(const std::string& product, const tBalanceUpdate& theChange, std::string& error);
    void UpdateBalance(const tBalanceUpdate& update);
    bool UpdateFutureBalance(const std::string& product, const tBalanceUpdate& update);
    //void ReloadFutureLimits(const std::string& product);
    void SetBalance(const tBalanceUpdate& update);
    void GetBalance(tBalanceUpdate& update);
    bool CanTradeFor(const std::string& tradeFor);

    void GetDropInfo(tDropInfo& dropInfo);

    inline double BuyingPower(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].BuyingPower;
    }

    inline double MaxDollars(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].MaxDollars;
    }

    inline int  MaxOrders(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].MaxOrders;
    }

    inline int  MaxShares(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].MaxShares;
    }

    inline long MaxTotalShares(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].MaxTotalShares;
    }

    inline double Balance(unsigned short secIndex, unsigned short sideIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].Balance[sideIndex];
    }

    inline int NumOrders(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].NumOrders;
    }

    inline long NumShares(unsigned short secIndex)
    {
        if (secIndex >= ROMVALUE::SECNUM)
        {
            secIndex = ROMVALUE::EQUITY;
        }
        return m_limits[secIndex].NumShares;
    }

    inline void SetBuyingPower(double value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].BuyingPower = value;
        }
    }

    inline void SetMaxDollars(double value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].MaxDollars = value;
        }
    }

    inline void SetMaxOrders(int value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].MaxOrders = value;
        }
    }

    inline void SetMaxShares(long value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].MaxShares = value;
        }
    }

    inline void SetMaxTotalShares(long value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].MaxTotalShares = value;
        }
    }

    inline void UpdateBalance(double value, unsigned short secIndex, unsigned short sideIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].Balance[sideIndex] += value;
        }
    }

    inline void UpdateNumOrders(int value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].NumOrders += value;
        }
    }

    inline void UpdateNumShares(long value, unsigned short secIndex)
    {
        if (secIndex < ROMVALUE::SECNUM)
        {
            m_limits[secIndex].NumShares += value;
        }
    }

    inline void SetLimits(const tLimits* limits)
    {
        for (int i = 0; i < ROMVALUE::SECNUM; i++)
        {
            m_limits[i] = limits[i];
        }
    }

    inline void SetLastLogonTime(int value)
    {
        m_lastlogontime = value;
    }

    inline void SetResetDay(int value)
    {
        m_resetday = value;
    }

    inline void LoadFutureLimits(const std::string* product)
    {
        m_futureLimits.ReLoad(product);
    }

    void GetFutureLimits(CRomMap<tLimits*>& map);
    void CopyInFutureLimits(const CUserFutureLimits& limits)
    {
        m_futureLimits = limits;
    }

    void CopyOutFutureBalance(CTraderData& data)
    {
        data.CopyInFutureBalance(m_futureLimits);
    }

    void CopyInFutureBalance(const CUserFutureLimits& limits)
    {
        const_cast<CUserFutureLimits&>(limits).CopyOutBalance(&m_futureLimits);
    }

    const CUserFutureLimits& FutureLimits() const { return m_futureLimits;}
    bool GetFutureLimits(const std::string& product, tLimits& limits);

    bool HasFutureProductLimits();
    
    const tHB& GetHB() const { return m_hb; }
    void SetHB(const tHB& hb) { m_hb = hb; }
    bool FindFuture(const std::string& product);
    
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

    tLimits m_limits[ROMVALUE::SECNUM];
    int m_resetday;
    time_t m_lastlogontime;
    int m_msgrateLimit[ROMVALUE::SECNUM];
    CUserFutureLimits m_futureLimits;
    
    //heart beat
    tHB m_hb;
};

class CThreadTraderData
{
public:
    CThreadTraderData(CTraderData* pData);
    ~CThreadTraderData();

    void UpdateBalance(const tBalanceUpdate& update);
    bool UpdateFutureBalance(const std::string& product, const tBalanceUpdate& update);
    void SetBalance(const tBalanceUpdate& update);
    void GetBalance(tBalanceUpdate& update);
    void GetTraderData(CTraderData* pData);
    void ExemptTrader(bool exempt);
    void GetPassword(std::string& pwd);
    void UpdatePassword(const std::string& pwd);
    bool CanTradeFor(const std::string& tradeFor);
    bool ExceedLimit_UserDefinedSpread(CRomOrderNew& order, const vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool ExceedLimit_ExchangeDefinedSpread(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool ExceedLimit_CoveredSpread(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool ExceedLimit(const tBalanceUpdate& change, std::string& error);
    bool ExceedFutureLimit(const std::string& product, const tBalanceUpdate& change, std::string& error);
    void GetDropInfo(tDropInfo& dropInfo);
    bool Exempted();
    const std::string Trader()
    {
        return m_data->Trader();
    }
    void GetResetDay(int& day);
    void GetLastLogonTime(time_t& t);
    void GetMsgRateLimit(int* rate);
    //void GetHBEnabled(bool& value);
    //void ReloadFutureLimits(const std::string& product);
    void GetFutureLimits(CRomMap<tLimits*>& map);
    void CopyOutFutureBalance(CThreadTraderData& data);
    void CopyInFutureBalance(const CTraderData& data);
    bool HasFutureProductLimits();
    bool FindFuture(const std::string& product);
    CTraderData* GetData() { return m_data; }  //the back door! It should only be 
                                               //accessed before the communications
                                               //to the outside starts!!!
    
private:
    CTraderData* m_data;
    pthread_rwlock_t m_lock;
};

class CClientProfileManagerNew
{
public:

    ~CClientProfileManagerNew();

    static CClientProfileManagerNew & getInstance()
    {
        static CClientProfileManagerNew theClientProfileManager;
        return theClientProfileManager;
    }

    static void* ReloadProfileFromDB(void* pArg = NULL);
    static void* ReloadClearingActSecLimits(void* pArg = NULL);
    static void* ReloadBillingGrpLimits(void* pArg = NULL);

    bool GetPassword(const std::string& login, std::string& password);
    bool UpdatePassword(const std::string& login, std::string& password);
    bool ValidateLogin(const std::string& login, const std::string& password, std::string& err);
    //void GetLoginList(ROMTYPEDEF::tLoginList& rLoginList);
    void GetTraderData(std::set<CTraderData*>& data);
    bool GetTraderData(const std::string& trader, CTraderData* pData);
    bool ExemptTrader(const std::string& key, bool action);
    bool ExemptClr(const std::string& key, bool action);
    bool ExemptBgrp(const std::string& key, bool action);

    void UpdateBalance(const tKeyedBalanceUpdate& update);
    void UpdateBalanceCoveredSpread(const tKeyedBalanceUpdate& update, tCoveredSpread* corder, bool bReplaceReject); 
    void UpdateBalanceNew(const tKeyedBalanceUpdate& update);
    void UpdateBalanceOld(const tKeyedBalanceUpdate& update);
    bool LoadProfileFromDB(const std::string* pTrader);
    bool LoadProfilesFromDB(bool reload = false);

    bool AllowedToPlaceOrder(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool AllowedToPlaceOrderNew(const CRomOrderNew& order, tBalanceUpdate& pBUpdate, std::string& error);
    bool AllowedToPlaceOrderOld(const CRomOrderNew& order, tBalanceUpdate& pBUpdate, std::string& error);
    bool AllowedToPlaceOrder_UserDefinedSpread(const CRomOrderNew& order, const vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool AllowedToPlaceOrder_ExchangeDefinedSpread(const CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error);
    bool AllowedToReplaceOrder(tKeyedBalanceUpdate& update, std::string& error);
    bool AllowedToReplaceCoveredSpreadOrder(tKeyedBalanceUpdate& update, long bodyshares, const tCoveredSpread* corder, std::string& error);

    //bool AllowedToReplaceOrderNew(tKeyedBalanceUpdate& update, std::string& error);
    //bool AllowedToReplaceOrderOld(tKeyedBalanceUpdate& update, std::string& error);
    void BackOutReplace(const CRomOrderNew& order);
    bool GetDropInfo(const std::string& trader, tDropInfo& dropInfo);
    bool GetClearingActSecLimits(const std::string& ClearingActSecKey, tLimits& limits);
    void ReloadClearingActSecLimits(const std::string* key = NULL);
    bool FindTrader(const std::string& trader);
    bool GetBillingGrpLimits(int billinggrpid, theLimits& limits);
    void ReloadBillingGrpLimits(const int* billinggrpid);
    
private:

    CClientProfileManagerNew();
    bool UpdateBalance(const std::string& trader, const tBalanceUpdate& update);
    bool UpdateFutureBalance(const std::string& trader, const std::string& product, const tBalanceUpdate& update);
    bool UpdateClearingActSecBalance(const std::string& key, const tBalanceUpdate& update);
    bool GetClientHB(tHB& hb, const std::string& clientname);
    
    bool loadClientHB(std::map<std::string, tHB>& hbMap, const std::string& trader, std::string& errStr);
    void packLoadProfileSqlStr(std::string& sqlstr, const std::string* trader);
    CTraderData* MakeTraderData(queue_t row);

    CRomMap<CThreadTraderData*> m_TraderToTraderData;
    //CRomMap<std::string> m_LoginToPassword;
    CClearingActLimits m_ClearingActSecLimits;
    CBillingGrpLimits m_BillingGrpLimits;
    pthread_rwlock_t m_lock;
};


#endif //__CLIENTPROFILEMANAGERNEW_H__
