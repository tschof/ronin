#include "ClientProfileManagerNew.h"
#include "Config.h"
#include "ClientsManager.h"
#include "debug_logger.h"
#include "BuffStore.h"
#include "MDSClient.h"
#include <math.h>

void CTraderData::init()
{
    m_hb.enabled = false;
    m_hb.interval = 30;
    m_hb.allowedmisses = 3;
    m_NumAccounts = 0;
    m_PrincipalAbility = false;
    m_DiscretionaryAbility = false;
    m_Exempt = false;
    m_RoninMember = false;
    m_ReportingType = 1;
    m_lastlogontime = 0;

    memset(&m_limits, 0, sizeof(m_limits));

    for (int i = 0; i < ROMVALUE::SECNUM; i++)
    {
        m_msgrateLimit[i] = -1;
    }
}
        
void CTraderData::GetDropInfo(tDropInfo& dropInfo)
{
    dropInfo.ReportingFirm = m_ReportingFirm;
    dropInfo.ReportingType = m_ReportingType;
    dropInfo.RoninMember = m_RoninMember;
}

bool CTraderData::CanTradeFor(const std::string& tradeFor)
{
    bool rc = true;

    if (OtherTraders().compare("N") == 0 ||
        (OtherTraders().compare("C") == 0 &&
         IsSubTrader(tradeFor.c_str()) == false))
    {
        rc = false;
    }

    return rc;
}

void CTraderData::Get(CTraderData* pData)
{
    pData->SetTrader(m_Trader);
    pData->SetPassword(m_Password);
    pData->SetGroup(m_Group);
    pData->SetReportingFirm(m_ReportingFirm);
    pData->SetRoninMember(m_RoninMember);
    pData->SetOtherTraders(m_OtherTraders);
    pData->LoadSubTraders(m_SubTraderSet);
    pData->SetExempt(m_Exempt);
    pData->SetNumAccounts(m_NumAccounts);
    pData->SetPrincipalAbility(m_PrincipalAbility);
    pData->SetDiscretionaryAbility(m_DiscretionaryAbility);
    pData->SetReportingType(m_ReportingType);

    pData->SetLimits(&(m_limits[0]));
    pData->CopyInFutureLimits(m_futureLimits);

    for (int i = 0; i < ROMVALUE::SECNUM; i++)
    {
        pData->SetMsgRateLimit(m_msgrateLimit[i], i);
    }
}

void CTraderData::GetBalance(tBalanceUpdate& update)
{
    update.dollarChange = m_limits[update.secIndex].Balance[update.sideIndex];
    update.shareChange = m_limits[update.secIndex].NumShares;
    update.orderChange = m_limits[update.secIndex].NumOrders;
}

bool CTraderData::IsSubTrader(const char* trader)
{
    tStringSet::iterator it = m_SubTraderSet.find(trader);
    if (it != m_SubTraderSet.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CTraderData::UpdateBalance(const tBalanceUpdate& update)
{
    m_limits[update.secIndex].Balance[update.sideIndex] += update.dollarChange;
    m_limits[update.secIndex].NumOrders += update.orderChange;
    m_limits[update.secIndex].NumShares += update.shareChange;
}

void CTraderData::SetBalance(const tBalanceUpdate& update)
{
    m_limits[update.secIndex].Balance[update.sideIndex] = update.dollarChange;
    m_limits[update.secIndex].NumOrders = update.orderChange;
    m_limits[update.secIndex].NumShares = update.shareChange;
}

bool CTraderData::UpdateFutureBalance(const std::string& product, const tBalanceUpdate& update)
{
    return m_futureLimits.UpdateBalance(product, update);
}

void CTraderData::LoadSubTraders(std::string traders)
{
    std::vector<std::string> subtraders;

    //std::string::size_type ps = traders.find(',');
    //std::string subtrader;
    CUtility::spliter(traders, subtraders, ',');

    for (unsigned int i = 0; i < subtraders.size(); i++)
    {
        m_SubTraderSet.insert( subtraders[i] );
    }

    /*if (ps == std::string::npos && traders.empty() == false)
    {
        m_SubTraderSet.insert( traders );
    }

    while (ps != std::string::npos)
    {
        subtrader = traders.substr(0, ps);
        m_SubTraderSet.insert( subtrader );

        if (traders.size() == ps + 1)
        {
            break;
        }

        traders = traders.substr(ps + 1);
        ps = traders.find(',');
    }

    if (traders.empty() == false)
    {
        m_SubTraderSet.insert( traders );
    }*/
}

void CTraderData::LoadSubTraders(tStringSet& traderSet)
{
    tStringSet::iterator it = traderSet.begin();

    while (it != traderSet.end())
    {
        m_SubTraderSet.insert(*it);
        it++;
    }
}

bool CTraderData::ExceedLimit(const tBalanceUpdate& theChange, std::string& error)
{
    //tLimits* pLimits = GetLimits(theChange.secType);
    double dollarChange = theChange.dollarChange;
    char errMsg[128];
    std::string secStr;
    CUtility::GetSecTypeStr(theChange.secIndex, secStr);

    if (m_Exempt)
    {
        m_limits[theChange.secIndex].Balance[theChange.sideIndex] += dollarChange;
        m_limits[theChange.secIndex].NumOrders += theChange.orderChange;
        m_limits[theChange.secIndex].NumShares += theChange.shareChange;
        return false;
    }

    int newNumOrders = m_limits[theChange.secIndex].NumOrders + theChange.orderChange;
    long newNumShares = m_limits[theChange.secIndex].NumShares + theChange.shareChange;

    //MaxOrder
    if (m_limits[theChange.secIndex].MaxOrders < newNumOrders)
    {
        sprintf(errMsg, "Exceeds %s's %s MaxOrders:%d", m_Trader.c_str(), secStr.c_str(), m_limits[theChange.secIndex].MaxOrders);
        error = errMsg;
        return true;
    }

    //MaxShares
    if (theChange.shareChange > m_limits[theChange.secIndex].MaxShares)
    {
        sprintf(errMsg, "Order's Shares Exceeds %s's %s Max Shares:%ld", m_Trader.c_str(), secStr.c_str(), m_limits[theChange.secIndex].MaxShares);
        error = errMsg;
        return true;
    }

    //MaxDollars
    if (dollarChange > m_limits[theChange.secIndex].MaxDollars)
    {
        sprintf(errMsg, "Order Exceeds %s's %s Max Dollars Allowed Per Order:%f", m_Trader.c_str(), secStr.c_str(), m_limits[theChange.secIndex].MaxDollars);
        error = errMsg;
        return true;
    }

    //Balance
    double newBalance = m_limits[theChange.secIndex].Balance[theChange.sideIndex] + dollarChange;

    if (newBalance > m_limits[theChange.secIndex].BuyingPower ||
        newBalance < -1 * m_limits[theChange.secIndex].BuyingPower)
    {
        sprintf(errMsg, "Exceeds %s's %s Buying Power:%f", m_Trader.c_str(), secStr.c_str(), m_limits[theChange.secIndex].BuyingPower);
        error = errMsg;
        return true;
    }

    //MaxTotalShares
    if (newNumShares > m_limits[theChange.secIndex].MaxTotalShares)
    {
        sprintf(errMsg, "Exceeds %s's %s Max Total Shares:%ld", m_Trader.c_str(), secStr.c_str(), m_limits[theChange.secIndex].MaxTotalShares);
        error = errMsg;
        return true;
    }

    m_limits[theChange.secIndex].Balance[theChange.sideIndex] = newBalance;
    m_limits[theChange.secIndex].NumOrders = newNumOrders;
    m_limits[theChange.secIndex].NumShares = newNumShares;

    return false;
}

bool CTraderData::ExceedFutureLimit(const std::string& product, const tBalanceUpdate& theChange, std::string& error)
{
    return m_futureLimits.ExceedLimit(product, theChange, m_Exempt, error);
}
/*
void CTraderData::ReloadFutureLimits(const std::string& product)
{
    std::string pd = product;
    std::string* pKey = (pd.empty())? NULL : &pd;
    m_futureLimits.ReloadFromDB(pKey);
}
*/
void CTraderData::GetFutureLimits(CRomMap<tLimits*>& map)
{
    m_futureLimits.CopyOutLimitsMap(map);
}

bool CTraderData::GetFutureLimits(const std::string& product, tLimits& limits)
{
    return m_futureLimits.GetLimits(product, limits);
}

bool CTraderData::HasFutureProductLimits()
{
    bool rc = (m_futureLimits.IsEmpty())? false : true;
    return rc;
}

bool CTraderData::FindFuture(const std::string& product)
{
    bool rc = m_futureLimits.Find(product);
    return rc;
}

CThreadTraderData::CThreadTraderData(CTraderData* pData) :
m_data(pData)
{
    pthread_rwlock_init(&m_lock, NULL);
    m_data->LoadFutureLimits(NULL);
}

CThreadTraderData::~CThreadTraderData()
{
    pthread_rwlock_wrlock(&m_lock);
    if (m_data)
    {
        delete m_data;
    }
    pthread_rwlock_unlock(&m_lock);

    pthread_rwlock_destroy(&m_lock);
}

void CThreadTraderData::GetDropInfo(tDropInfo& dropInfo)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->GetDropInfo(dropInfo);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::ExceedLimit(const tBalanceUpdate& change, std::string& error)
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data->ExceedLimit(change, error);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CThreadTraderData::ExceedFutureLimit(const std::string& product, const tBalanceUpdate& change, std::string& error)
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data->ExceedFutureLimit(product, change, error);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CThreadTraderData::HasFutureProductLimits()
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data->HasFutureProductLimits();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}


bool CThreadTraderData::FindFuture(const std::string& product)
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data->FindFuture(product);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}
/*
bool CThreadTraderData::ExceedLimit(const CRomOrderNew& order, tBalanceUpdate& BUpdate, std::string& error)
{
    BUpdate.orderChange = 1;

    BUpdate.shareChange = order.Shares();
    BUpdate.secIndex = order.SecIndex();
    BUpdate.sideIndex = order.SideIndex();

    double price = order.Price();

    if (order.CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
    {
        price = fabs(price);
    }
    BUpdate.dollarChange = BUpdate.shareChange * price * order.Multiplier();

    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data.ExceedLimit(BUpdate, error);
    pthread_rwlock_unlock(&m_lock);

    return rc;
}
*/
bool CThreadTraderData::ExceedLimit_UserDefinedSpread(CRomOrderNew& order, const vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    //CCsv* pChild = NULL;
    bool rc = false;
    //int ratio = 0, total_ratio = 0;

    const tCoveredSpread* pCoveredSpread = order.CoveredSpread();
    if (pCoveredSpread)
    {
        rc = ExceedLimit_CoveredSpread(order, balances, error);
    }
    else
    {
        int ratio = 0, total_ratio = 0;

        CCsv* pChild = (*pLegs)[0];

        for (size_t i = 0; i < pLegs->size(); i++)
        {
            pChild = (*pLegs)[i];

            if (pChild->GetAt(ROMFIELDS::CSV_SHARES, ratio))
            {
                total_ratio += ratio;
            }
        }

        tBalanceUpdate* pBU = new tBalanceUpdate;
       
        pBU->secIndex = order.SecIndex();
        pBU->sideIndex = order.SideIndex();
        pBU->orderChange = 1;
        pBU->shareChange = total_ratio * order.Shares();
        pBU->dollarChange = order.Multiplier() * order.Shares() * fabs(order.Price());

        order.SetRatio(total_ratio);
        
        rc = ExceedLimit(*pBU, error);
        if (false == rc)
        {
            balances.push_back(pBU);
        }
    }

    //total cash contribution, assuming that currency rate is one
    /*BUpdate.secIndex = order.SecIndex();
    BUpdate.sideIndex = order.SideIndex();
    BUpdate.orderChange = 1;
    BUpdate.shareChange = total_ratio * order.Shares();
    BUpdate.dollarChange = order.Multiplier() * order.Shares() * fabs(order.Price());

    rc = ExceedLimit(BUpdate, error);
    order.SetRatio(total_ratio);*/

    return rc;
}

bool CThreadTraderData::ExceedLimit_ExchangeDefinedSpread(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    bool rc = false;
   
    const tCoveredSpread* pCoveredSpread = order.CoveredSpread();
    if (pCoveredSpread)
    {
        rc = ExceedLimit_CoveredSpread(order, balances, error);
    }
    else
    {
        tBalanceUpdate* pBU = new tBalanceUpdate;
        pBU->secIndex = order.SecIndex();
        pBU->sideIndex = order.SideIndex();
        pBU->orderChange = 1;
        pBU->shareChange = order.Shares();
        pBU->dollarChange = order.Multiplier() * order.Shares() * fabs(order.Price());

        rc = ExceedLimit(*pBU, error);
        
        if (false == rc)
        {
            balances.push_back(pBU);
        }
    }

    return rc;
}

void CThreadTraderData::UpdateBalance(const tBalanceUpdate& update)
{
    pthread_rwlock_wrlock(&m_lock);
    m_data->UpdateBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::UpdateFutureBalance(const std::string& product, const tBalanceUpdate& update)
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data->UpdateFutureBalance(product, update);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CThreadTraderData::SetBalance(const tBalanceUpdate& update)
{
    pthread_rwlock_wrlock(&m_lock);
    m_data->SetBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetBalance(tBalanceUpdate& update)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->GetBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::CanTradeFor(const std::string& tradeFor)
{
    bool rt = false;
    pthread_rwlock_rdlock(&m_lock);
    rt = m_data->CanTradeFor(tradeFor);
    pthread_rwlock_unlock(&m_lock);
    return rt;
}

void CThreadTraderData::ExemptTrader(bool exempt)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->SetExempt(exempt);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::Exempted()
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_data->Exempt();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CThreadTraderData::GetPassword(std::string& pwd)
{
    pthread_rwlock_rdlock(&m_lock);
    pwd = m_data->Password();
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::UpdatePassword(const std::string& pwd)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->SetPassword(pwd);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetTraderData(CTraderData* pData)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->Get(pData);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetResetDay(int& day)
{
    pthread_rwlock_rdlock(&m_lock);
    day = m_data->ResetDay();
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetLastLogonTime(time_t& t)
{
    pthread_rwlock_rdlock(&m_lock);
    t = m_data->LastLogonTime();
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetMsgRateLimit(int* rate)
{
    pthread_rwlock_rdlock(&m_lock);
    for (int i = 0; i < ROMVALUE::SECNUM; i++)
    {
        rate[i] = m_data->msgrateLimit(i);
    }
    pthread_rwlock_unlock(&m_lock);
}

/*
void CThreadTraderData::ReloadFutureLimits(const std::string& product)
{
    pthread_rwlock_wrlock(&m_lock);
    m_data->ReloadFutureLimits(product);
 * 
    pthread_rwlock_unlock(&m_lock);
}
*/
void CThreadTraderData::GetFutureLimits(CRomMap<tLimits*>& map)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data->GetFutureLimits(map);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::CopyOutFutureBalance(CThreadTraderData& data)
{
    pthread_rwlock_rdlock(&m_lock);
    data.CopyInFutureBalance(*m_data);
    //m_data->CopyOutFutureBalance(data);
    pthread_rwlock_unlock(&m_lock);
}

/*void CThreadTraderData::GetHBEnabled(bool& value)
{
    pthread_rwlock_rdlock(&m_lock);
    value = m_data->HBEnalbed();
    pthread_rwlock_unlock(&m_lock);
}*/

void CThreadTraderData::CopyInFutureBalance(const CTraderData& data)
{
    m_data->CopyInFutureBalance(data.FutureLimits());
}

CClientProfileManagerNew::CClientProfileManagerNew() : m_TraderToTraderData(true)
{
    pthread_rwlock_init(&m_lock, NULL);
    LoadProfilesFromDB();
    m_ClearingActSecLimits.ReloadFromDB();
}

CClientProfileManagerNew::~CClientProfileManagerNew()
{
    std::list<CThreadTraderData*> dataList;
    m_TraderToTraderData.GetAll(dataList);
    std::list<CThreadTraderData*>::iterator it = dataList.begin();
    CThreadTraderData* ptData = NULL;

    while (it != dataList.end())
    {
        ptData = *it;
        delete ptData;
        it++;
    }

    pthread_rwlock_destroy(&m_lock);
}

bool CClientProfileManagerNew::GetPassword(const std::string& login, std::string& password)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(login, ptData))
    {
        ptData->GetPassword(password);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManagerNew::UpdatePassword(const std::string& login, std::string& password)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(login, ptData))
    {
        ptData->UpdatePassword(password);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}
/*
bool CClientProfileManagerNew::ValidateLogin(const std::string& login, const std::string& password, std::string& err)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    std::string pwd;

    if (m_LoginToPassword.Get(login, pwd))
    {
        if (pwd != password)
        {
            err = "invalid password";
        }
        else
        {
            rc = true;
        }
    }
    else
    {
        rc = false;
        err = "invalid login";
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}
*/
/*
void CClientProfileManagerNew::GetLoginList(ROMTYPEDEF::tLoginList& rLoginList)
{
    pthread_rwlock_rdlock(&m_lock);

    std::list<CThreadTraderData*> traderDataList;
    m_TraderToTraderData.GetAll(traderDataList);
    std::list<CThreadTraderData*>::iterator it = traderDataList.begin();
    ROMTYPEDEF::tLoginStruct login;
    //std::list<CRomMap<std::string>::tKeyValuePair>::iterator it = keyvalues.begin();
    while (it != traderDataList.end())
    {
        login.login = (*it)->Trader();
        (*it)->GetPassword(login.password);
        (*it)->GetResetDay(login.resetday);
        (*it)->GetLastLogonTime(login.lastLogonTime);
        (*it)->GetMsgRateLimit(&(login.msgrateLimit[0]));
        (*it)->GetHBEnabled(login.hbEnabled);
        rLoginList.push_back(login);
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}
*/
void CClientProfileManagerNew::GetTraderData(std::set<CTraderData*>& data)
{
    pthread_rwlock_rdlock(&m_lock);

    std::list<CThreadTraderData*> traderDataList;
    m_TraderToTraderData.GetAll(traderDataList);
    std::list<CThreadTraderData*>::iterator it = traderDataList.begin();
    
    while (it != traderDataList.end())
    {
        data.insert((*it)->GetData());
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}

bool CClientProfileManagerNew::GetTraderData(const std::string& trader, CTraderData* pData)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;

    if (m_TraderToTraderData.Get(trader, ptData))
    {
        ptData->GetTraderData(pData);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManagerNew::GetDropInfo(const std::string& trader, tDropInfo& dropInfo)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;

    if (m_TraderToTraderData.Get(trader, ptData))
    {
        ptData->GetDropInfo(dropInfo);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManagerNew::ExemptTrader(const std::string& key, bool action)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(key, ptData))
    {
        ptData->ExemptTrader(action);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManagerNew::ExemptClr(const std::string& key, bool action)
{
    bool rc = m_ClearingActSecLimits.Exempt(key, action);
    return rc;
}

bool CClientProfileManagerNew::ExemptBgrp(const std::string& key, bool action)
{
    bool rc = m_BillingGrpLimits.Exempt(atoi(key.c_str()), action);
    return rc;
}
    
bool CClientProfileManagerNew::UpdateBalance(const std::string& trader, const tBalanceUpdate& update)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(trader, ptData))
    {
        ptData->UpdateBalance(update);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManagerNew::UpdateFutureBalance(const std::string& trader, const std::string& product, const tBalanceUpdate& update)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(trader, ptData))
    {
        if (ptData->HasFutureProductLimits())
        {
            ptData->UpdateFutureBalance(product, update);
        }
        else
        {
            UpdateBalance(trader, update);
        }
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}


void CClientProfileManagerNew::UpdateBalance(const tKeyedBalanceUpdate& update)
{
    if (ROMVALUE::FUTURE == update.update.secIndex)
    {     
        UpdateFutureBalance(update.username, update.product, update.update);
    }
    else
    {
        UpdateBalance(update.username, update.update);
    }
    
    if (update.update.secIndex != ROMVALUE::FUTURE &&
        update.update.secIndex != ROMVALUE::OPTIONONFUTURE)
    {
        if (m_BillingGrpLimits.UpdateBalance(update.clract, update.update) == false)
        {
            m_ClearingActSecLimits.UpdateBalance(update.clearing_sec, update.update);
        }
    }
    else
    {
        m_ClearingActSecLimits.UpdateBalance(update.clearing_sec, update.update);
    }
}


void* CClientProfileManagerNew::ReloadProfileFromDB(void* pArg)
{
    std::string* pKey = (std::string*)pArg;

    if (pKey)
    {
        CClientProfileManagerNew::getInstance().LoadProfileFromDB(pKey);
        //CClientProfileManagerNew::getInstance().ReloadFutureLimits(*pKey, "");

        delete pKey;
    }
    else
    {
        CClientProfileManagerNew::getInstance().LoadProfilesFromDB(true);
        //CClientProfileManagerNew::getInstance().ReloadAllFutureLimits();
    }
    return NULL;
}

CTraderData* CClientProfileManagerNew::MakeTraderData(queue_t row)
{
    gendb_v* pVal = NULL;
    element_t colE;
    int cnt = 0;
    CTraderData* pData = NULL;
    std::string strvalue;

    while (is_empty(row) == 0)
    {
        colE = pop_front(row);
        pVal = (gendb_v*)(colE->d);

        if (pVal->type_in_union != DB_NULL)
        {
            switch(cnt)
            {
            case 0:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData = new CTraderData(strvalue);
                }
                break;
            case 1:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData->SetPassword(strvalue);
                }
                break;
            case 4:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData->SetReportingFirm(strvalue);
                }
                break;
            case 5:
                pData->SetReportingType((pVal->g_un).un_int);
                break;
            case 6:
                pData->SetRoninMember((pVal->g_un).un_int);
                break;
            case 7:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData->LoadSubTraders(strvalue);
                }
                break;
            case 9:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData->SetOtherTraders(strvalue);
                }
                break;
            case 10:
                if ((pVal->g_un).un_int == 0)
                {
                    pData->SetDiscretionaryAbility(false);
                }
                else
                {
                    pData->SetDiscretionaryAbility(true);
                }
                break;
            case 11:
                if ((pVal->g_un).un_int == 0)
                {
                    pData->SetPrincipalAbility(false);
                }
                else
                {
                    pData->SetPrincipalAbility(true);
                }
                break;
            case 12:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pData->SetGroup(strvalue);
                }
                break;
            case 13:
                pData->SetBuyingPower((pVal->g_un).un_double, ROMVALUE::EQUITY);
                break;
            case 14:
                pData->SetBuyingPower((pVal->g_un).un_double, ROMVALUE::OPTION);
                break;
            case 15:
                pData->SetBuyingPower((pVal->g_un).un_double, ROMVALUE::FUTURE);
                break;
            case 16:
                pData->SetBuyingPower((pVal->g_un).un_double, ROMVALUE::OPTIONONFUTURE);
                break;
            case 17:
                pData->SetMaxShares((pVal->g_un).un_int, ROMVALUE::EQUITY);
                break;
            case 18:
                pData->SetMaxShares((pVal->g_un).un_int, ROMVALUE::OPTION);
                break;
            case 19:
                pData->SetMaxShares((pVal->g_un).un_int, ROMVALUE::FUTURE);
                break;
            case 20:
                pData->SetMaxShares((pVal->g_un).un_int, ROMVALUE::OPTIONONFUTURE);
                break;
            case 21:
                pData->SetMaxDollars((pVal->g_un).un_double, ROMVALUE::EQUITY);
                break;
            case 22:
                pData->SetMaxDollars((pVal->g_un).un_double, ROMVALUE::OPTION);
                break;
            case 23:
                pData->SetMaxDollars((pVal->g_un).un_double, ROMVALUE::FUTURE);
                break;
            case 24:
                pData->SetMaxDollars((pVal->g_un).un_double, ROMVALUE::OPTIONONFUTURE);
                break;
            case 25:
                pData->SetMaxOrders((pVal->g_un).un_int, ROMVALUE::EQUITY);
                break;
            case 26:
                pData->SetMaxOrders((pVal->g_un).un_int, ROMVALUE::OPTION);
                break;
            case 27:
                pData->SetMaxOrders((pVal->g_un).un_int, ROMVALUE::FUTURE);
                break;
            case 28:
                pData->SetMaxOrders((pVal->g_un).un_int, ROMVALUE::OPTIONONFUTURE);
                break;
            case 29:
                pData->SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::EQUITY);
                break;
            case 30:
                pData->SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::OPTION);
                break;
            case 31:
                pData->SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::FUTURE);
                break;
            case 32:
                pData->SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::OPTIONONFUTURE);
                break;
            case 33:
                pData->SetResetDay((pVal->g_un).un_int);
                break;
            case 34:
                pData->SetLastLogonTime((pVal->g_un).un_int);
                break;
            case 35:
                if (pVal->type_in_union != DB_NULL &&
                    (pVal->g_un).un_int != 0)
                {
                    pData->SetMsgRateLimit((pVal->g_un).un_int, ROMVALUE::EQUITY);
                }
                break;
            case 36:
                if (pVal->type_in_union != DB_NULL &&
                    (pVal->g_un).un_int != 0)
                {
                    pData->SetMsgRateLimit((pVal->g_un).un_int, ROMVALUE::OPTION);
                }
                break;
            case 37:
                if (pVal->type_in_union != DB_NULL &&
                    (pVal->g_un).un_int != 0)
                {
                    pData->SetMsgRateLimit((pVal->g_un).un_int, ROMVALUE::FUTURE);
                }
                break;
            case 38:
                if (pVal->type_in_union != DB_NULL &&
                    (pVal->g_un).un_int != 0)
                {
                    pData->SetMsgRateLimit((pVal->g_un).un_int, ROMVALUE::OPTIONONFUTURE);
                }
                break;
            default:
                break;
            }
        }

        cnt++;
        free(colE);
        gendb_destroy_v(pVal);
    }

    return pData;
}

void CClientProfileManagerNew::packLoadProfileSqlStr(std::string& sqlstr, const std::string* trader)
{
    std::string romname = CConfig::getInstance().Login();

    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    databuf_write(pBuffer, "CREATE TEMPORARY TABLE IF NOT EXISTS sublist "
                           "(supertrader character varying, subtraders character varying); "
                           "TRUNCATE sublist; INSERT INTO sublist SELECT supertrader, "
                           "string_agg(subtrader, ',') FROM traderhierarchy "
                           "GROUP BY supertrader; ");
    
    databuf_write(pBuffer, "SELECT t2.username, t2.password, t2.token, t2.omuserid, t5.firmfixlogin, "
            "t5.reporttype, t5.roninmember, t6.subtraders, t1.traderacr, t1.othertraders, "
            "t1.discretionaryorders, t1.principalorders, t1.groupacr, t1.EquityBuyingPower, "
            "t1.OptionBuyingPower, t1.FutureBuyingPower, t1.OFBuyingPower, "
            "t1.EquityMaxShares, t1.OptionMaxContracts, t1.FutureMaxContracts, "
            "t1.OFMaxContracts, t1.EquityMaxDollars, t1.OptionMaxDollars, "
            "t1.FutureMaxDollars, t1.OFMaxDollars, t1.EquityMaxOrders, "
            "t1.OptionMaxOrders, t1.FutureMaxOrders, t1.OFMaxOrders, "
            "t1.EquityTotalMaxShares, t1.OptionTotalMaxContracts, "
            "t1.FutureTotalMaxContracts, t1.OFTotalMaxContracts, "
            "t2.resetday, "); 

    databuf_write(pBuffer, "(SELECT logon_time_t FROM clientlastlogon WHERE"
                           " server='%s' AND login=t2.username), ", romname.c_str());

    databuf_write(pBuffer, "t1.msgratelimit_e, t1.msgratelimit_o,"
            "t1.msgratelimit_f, t1.msgratelimit_of, t2.reset_hour, t2.reset_min from TraderOrderLimitsNew t1  "
            "join OMuser t2 on t2.username = t1.TraderAcr left join OMUserCustomer t3 "
            "on t2.OMuserID = t3.userID left join OMCustomer t4 "
            "on t3.CustomerID = t4.OMCustomerID left join ombillingfirm t5 "
            "on t4.ombillingfirmid = t5.ombillingfirmid "
            "left join sublist AS t6 on t6.supertrader = t2.username");
    
    if (trader)
    {
        databuf_write(pBuffer, "  WHERE t2.username"
                " = '%s'", trader->c_str());
    }

    sqlstr.assign(pBuffer->buffer, databuf_unread(pBuffer));
    CBuffStore::getInstance().ReturnB(pBuffer);
}

bool CClientProfileManagerNew::LoadProfileFromDB(const std::string* pTrader)
{
    bool rc = true;
    queue_t results = create_queue();

    std::string sqlStr;
    packLoadProfileSqlStr(sqlStr, pTrader);
    //const tDBLogin& db = CConfig::getInstance().db();
    const std::string& rom = CConfig::getInstance().Login();
    //const std::string& proc = CConfig::getInstance().ProcLoadRomUsers();
    std::string errStr;
        
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        element_t rowE;
                
        if (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            CTraderData* pData = MakeTraderData(row);
            
            tHB hb = {false, 30, 3};
            if (GetClientHB(hb, pData->Trader()))
            {
                pData->SetHB(hb);
            }
                       
            free(rowE);
            destroy_queue(row);
            CThreadTraderData* pTData = new CThreadTraderData(pData);
            CThreadTraderData* ptoTrader = NULL;

            if (m_TraderToTraderData.Get(pData->Trader(), ptoTrader) == false)
            {
                m_TraderToTraderData.Add(pData->Trader(), pTData);
                CClientsManager::getInstance().AddNewClient(*pData);
            }
            else
            {
                tBalanceUpdate tbdata;

                for (int i = ROMVALUE::EQUITY; i < ROMVALUE::SECNUM; i++)
                {
                    tbdata.secIndex = i;
                    for (int j = ROMVALUE::BUY; j < ROMVALUE::SIDENUM; j++)
                    {
                        tbdata.sideIndex = j;
                        ptoTrader->GetBalance(tbdata);
                        pTData->SetBalance(tbdata);
                    }
                }

                pTData->ExemptTrader(ptoTrader->Exempted());

                std::string password;
                ptoTrader->GetPassword(password);

                if (password.compare(pData->Password()) != 0)
                {
                    CClientsManager::getInstance().UpdateClientPassword(pData->Trader(), pData->Password());
                }

                ptoTrader->CopyOutFutureBalance(*pTData);
                m_TraderToTraderData.Update(pData->Trader(), pTData);

                //need to update message rate in CClientConnection!!
                CClientsManager::getInstance().UpdateMsgRateLimits(pData->Trader(), pData->msgrateLimit());
                CClientsManager::getInstance().UpdateHB(*pData);
                delete ptoTrader;
            }
        }
        else
        {
            char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
            sprintf(errMsg, "%spSqlstr:%s\n", sqlStr.c_str(), errStr.c_str());
            send_debug_message(errMsg);
            free(errMsg);
        }

        destroy_queue(results);
    }

    return rc;
}

bool CClientProfileManagerNew::LoadProfilesFromDB(bool reload)
{
    bool rc = true;
    queue_t results = create_queue();
    //const tDBLogin& db = CConfig::getInstance().db();
    const std::string& rom = CConfig::getInstance().Login();
    //const std::string& proc = CConfig::getInstance().ProcLoadRomUsers();

    //std::set<std::string> hbUsers;
    std::map<std::string, tHB> hbmap;
    std::string dumy, hberror;
    
    if (loadClientHB(hbmap, dumy, hberror) == false)
    {
        //some error
    }
    
    std::string errStr;
    std::string sqlStr;
    packLoadProfileSqlStr(sqlStr, NULL);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        element_t rowE;
        CRomMap<CThreadTraderData*> tempTraderToTraderData(false);

        //char msg[128];
        //sprintf(msg, "<%s> was executed", sqlStr.c_str());
        //send_debug_message(msg);
        
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            CTraderData* pData = MakeTraderData(row);
            std::map<std::string, tHB>::iterator it = hbmap.find(pData->Trader());
            if (it != hbmap.end())
            {
                pData->SetHB(it->second);
            }
                       
            free(rowE);
            destroy_queue(row);

            CThreadTraderData* ptData = new CThreadTraderData(pData);
            tempTraderToTraderData.Add(pData->Trader(), ptData);

            if (true == reload)
            {
                CThreadTraderData* ptmpData = NULL;
                if (m_TraderToTraderData.Get(pData->Trader(), ptmpData) == false)
                {
                    CClientsManager::getInstance().AddNewClient(*pData);
                }
                else
                {
                    std::string pwd;
                    ptmpData->GetPassword(pwd);
                    if (pData->Password().compare(pwd) != 0)
                    {
                        CClientsManager::getInstance().UpdateClientPassword(pData->Trader(), pData->Password());
                    }

                    CClientsManager::getInstance().UpdateMsgRateLimits(pData->Trader(), pData->msgrateLimit());
                    CClientsManager::getInstance().UpdateHB(*pData);
                }
            }
        }

        pthread_rwlock_wrlock(&m_lock);

        m_TraderToTraderData.Swap(tempTraderToTraderData);

        if (reload)
        {
            std::list<CThreadTraderData*> oldData;

            tempTraderToTraderData.GetAll(oldData);

            std::list<CThreadTraderData*>::iterator it = oldData.begin();
            CThreadTraderData *ptTrader = NULL;
            tBalanceUpdate tbdata;

            //resume current balance
            while (it != oldData.end())
            {
                ptTrader = *it;
                CThreadTraderData* ptData = NULL;

                if (m_TraderToTraderData.Get(ptTrader->Trader(), ptData))
                {
                    for (int i = ROMVALUE::EQUITY; i < ROMVALUE::SECNUM; i++)
                    {
                        tbdata.secIndex = i;
                        for (int j = ROMVALUE::BUY; j < ROMVALUE::SIDENUM; j++)
                        {
                            tbdata.sideIndex = j;
                            ptTrader->GetBalance(tbdata);
                            ptData->SetBalance(tbdata);
                        }
                    }

                    std::string oldpwd, newpwd;
                    ptTrader->GetPassword(oldpwd);
                    ptData->GetPassword(newpwd);
                    ptData->ExemptTrader(ptTrader->Exempted());
                    ptTrader->CopyOutFutureBalance(*ptData);
                }
                it++;
                delete ptTrader;
            }
        }

        pthread_rwlock_unlock(&m_lock);
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
        rc = false;
    }

    destroy_queue(results);

    return rc;
}


bool CClientProfileManagerNew::AllowedToPlaceOrder(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    bool rc = true;
    CThreadTraderData* pData = NULL;

    pthread_rwlock_rdlock(&m_lock);

    tBalanceUpdate* pBalance = new tBalanceUpdate;
    tBalanceUpdate& BUpdate = *pBalance;

    BUpdate.sideIndex = order.SideIndex();

    if (m_TraderToTraderData.Get(order.Trader(), pData))
    {
        if (strcmp(order.Trader(), order.TradeFor()) != 0 &&
            pData->CanTradeFor(order.TradeFor()) == false)
        {
            error.append("Not Allowed to trade for ");
            error.append(order.TradeFor());
            rc = false;
        }
        else
        {
            BUpdate.shareChange = order.Shares();
            BUpdate.secIndex = order.SecIndex();
            BUpdate.sideIndex = order.SideIndex();
            BUpdate.orderChange = 1;

            double price = order.Price();
            bool mp = order.NeedEquityMidPrice();
            if (mp)
            {
                price = CMDSClient::getInstance().GetEquityMidPrice(order.Symbol());
            }

            if (order.CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
            {
                price = fabs(price);
            }
            
            if (mp)
            {
                order.SetmPrice(price);
            }
            
            BUpdate.dollarChange = BUpdate.shareChange * price * order.Multiplier();

            if (ROMVALUE::FUTURE == BUpdate.secIndex)
            {
                if (pData->HasFutureProductLimits())
                {
                    rc = !pData->ExceedFutureLimit(order.Product(), BUpdate, error);
                }
                else
                {
                    rc = !pData->ExceedLimit(BUpdate, error);
                }
            }
            else
            {
                rc = !pData->ExceedLimit(BUpdate, error);
            }
         }
    }
    else
    {
        error.append("Invalid Trader:");
        error.append(order.Trader());
        rc = false;
    }

    pthread_rwlock_unlock(&m_lock);

    //if (rc && CConfig::getInstance().ClearingLimitsEnabled())
    if (rc)
    {
        std::string clr = order.ClearingAccount();
        
        if (BUpdate.secIndex != ROMVALUE::FUTURE &&
            BUpdate.secIndex != ROMVALUE::OPTIONONFUTURE)
        {
            int rtc = m_BillingGrpLimits.ExceedLimit(clr, BUpdate, error);
            if (-1 == rtc)  //if the clr has the corresponding billing group limits skip ClearingAct limits check
            {
                rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), BUpdate, error));
            }
            else
            {
                rc = !rtc;
            }
        }
        else
        {
            rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), BUpdate, error));
        }
               
        if (false == rc)
        {
            //back out the balance update from the per trader balance
            BUpdate.orderChange *= -1;
            BUpdate.shareChange *= -1;
            BUpdate.dollarChange *= -1;
            if (ROMVALUE::FUTURE == BUpdate.secIndex)
            {
                std::string product = order.Product();
                UpdateFutureBalance(order.Trader(), product, BUpdate);
            }
            else
            {
                UpdateBalance(order.Trader(), BUpdate);
            }
        }
    }

    if (rc)
    {
        balances.push_back(pBalance);
    }
    else
    {
        delete pBalance;
    }
    
    return rc;
}


bool CClientProfileManagerNew::AllowedToPlaceOrder_UserDefinedSpread(const CRomOrderNew& order, const vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    bool rc = true;
    CThreadTraderData* pData = NULL;

    pthread_rwlock_rdlock(&m_lock);

    //tBalanceUpdate BUpdate;
    //BUpdate.sideIndex = order.SideIndex();

    if (m_TraderToTraderData.Get(order.Trader(), pData))
    {
        if (strcmp(order.Trader(), order.TradeFor()) != 0 &&
            pData->CanTradeFor(order.TradeFor()) == false)
        {
            error.append("Not Allowed to trade for ");
            error.append(order.TradeFor());
            rc = false;
        }
        else
        {
            rc = !pData->ExceedLimit_UserDefinedSpread(const_cast<CRomOrderNew&>(order), pLegs, balances, error);
        }
    }
    else
    {
        error.append("Invalid Trader:");
        error.append(order.Trader());
        rc = false;
    }

    pthread_rwlock_unlock(&m_lock);
    
    bool BillingTouched[2];
    BillingTouched[0] = false;
    BillingTouched[1] = false;
    std::string clr = order.ClearingAccount();

    if (rc)
    {
        //std::string clr = order.ClearingAccount();

        for (unsigned short i = 0; i < balances.size(); i++)
        {
            if (balances[i]->secIndex != ROMVALUE::FUTURE &&
                balances[i]->secIndex != ROMVALUE::OPTIONONFUTURE)
            {
                int rtc = m_BillingGrpLimits.ExceedLimit(clr, *(balances[i]), error);
                if (-1 == rtc)  //if the clr has the corresponding billing group limits skip ClearingAct limits check
                {
                    rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), *(balances[i]), error));
                }
                else
                {
                    rc = !rtc;
                    if (rc)
                    {
                        BillingTouched[i] = true;
                    }
                }
            }
            else
            {
                rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), *(balances[i]), error));
            }
            if (false == rc)
            {
                break;
            }
        }
    }
    
    if (false == rc)
    {
        //back out the balance update from the per trader balance
        for (unsigned short i = 0; i < balances.size(); i++)
        {
            balances[i]->orderChange *= -1;
            balances[i]->shareChange *= -1;
            balances[i]->dollarChange *= -1;
            if (ROMVALUE::FUTURE == balances[i]->secIndex)
            {
                std::string product = order.Product();
                UpdateFutureBalance(order.Trader(), product, *(balances[i]));
            }
            else
            {
                UpdateBalance(order.Trader(), *(balances[i]));
            }

            if (BillingTouched[i])
            {
                m_BillingGrpLimits.UpdateBalance(clr, *(balances[i]));
            }
        }
    }

    return rc;
}

bool CClientProfileManagerNew::AllowedToPlaceOrder_ExchangeDefinedSpread(const CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    bool rc = true;
    CThreadTraderData* pData = NULL;

    pthread_rwlock_rdlock(&m_lock);

    if (m_TraderToTraderData.Get(order.Trader(), pData))
    {
        if (strcmp(order.Trader(), order.TradeFor()) != 0 &&
            pData->CanTradeFor(order.TradeFor()) == false)
        {
            error.append("Not Allowed to trade for ");
            error.append(order.TradeFor());
            rc = false;
        }
        else
        {
            rc = !pData->ExceedLimit_ExchangeDefinedSpread(const_cast<CRomOrderNew&>(order), balances, error);
        }
    }
    else
    {
        error.append("Invalid Trader:");
        error.append(order.Trader());
        rc = false;
    }

    pthread_rwlock_unlock(&m_lock);
    
    bool BillingTouched[2];
    BillingTouched[0] = false;
    BillingTouched[1] = false;
    std::string clr = order.ClearingAccount();

    if (rc)
    {
        //std::string clr = order.ClearingAccount();

        for (unsigned short i = 0; i < balances.size(); i++)
        {
            if (balances[i]->secIndex != ROMVALUE::FUTURE &&
                balances[i]->secIndex != ROMVALUE::OPTIONONFUTURE)
            {
                int rtc = m_BillingGrpLimits.ExceedLimit(clr, *(balances[i]), error);
                if (-1 == rtc)  //if the clr has the corresponding billing group limits skip ClearingAct limits check
                {
                    rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), *(balances[i]), error));
                }
                else
                {
                    rc = !rtc;
                    if (rc)
                    {
                        BillingTouched[i] = true;
                    }
                }
            }
            else
            {
                rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrderNew&>(order).ClearingSecKey(), *(balances[i]), error));
            }

            if (false == rc)
            {
                break;
            }
        }

        if (false == rc)
        {
            //back out the balance update from the per trader balance
            for (unsigned short i = 0; i < balances.size(); i++)
            {
                balances[i]->orderChange *= -1;
                balances[i]->shareChange *= -1;
                balances[i]->dollarChange *= -1;
                if (ROMVALUE::FUTURE == balances[i]->secIndex)
                {
                    std::string product = order.Product();
                    UpdateFutureBalance(order.Trader(), product, *(balances[i]));
                }
                else
                {
                    UpdateBalance(order.Trader(), *(balances[i]));
                }

                if (BillingTouched[i])
                {
                    m_BillingGrpLimits.UpdateBalance(clr, *(balances[i]));
                }
            }
        }
    }

    return rc;
}

void CClientProfileManagerNew::BackOutReplace(const CRomOrderNew& order)
{
    tBalanceUpdate theChange = {1, 0, 0, order.SecIndex()};

    /*if (order.OptionOnFuture())
    {
        theChange.secType = ROMVALUE::SECTYPE_OPTIONFUTURE;
    }
    else
    {
        theChange.secType = order.SecType();
    }*/
    //theChange.sideSign = (order.Side() == ROMVALUE::SIDE_BUY)? 1:-1;
    theChange.shareChange = -1 * order.ShareChange();
    theChange.orderChange = 0;

    theChange.dollarChange = -1 * order.Multiplier() * order.CurrencyRate() * order.SideSign() * (order.PriceChange() * (order.Leave() + order.ShareChange()) + order.ShareChange() * order.Price());

    CThreadTraderData* pData = NULL;

    pthread_rwlock_rdlock(&m_lock);

    if (m_TraderToTraderData.Get(order.Trader(), pData))
    {
        pData->UpdateBalance(theChange);
    }

    pthread_rwlock_unlock(&m_lock);

    m_ClearingActSecLimits.UpdateBalance(const_cast<CRomOrderNew&>(order).ClearingSecKey(), theChange);
}


bool CClientProfileManagerNew::AllowedToReplaceOrder(tKeyedBalanceUpdate& update, std::string& error)
{
    CThreadTraderData* pData = NULL;
    bool rc = true;

    pthread_rwlock_rdlock(&m_lock);

    if (m_TraderToTraderData.Get(update.username, pData))
    {
        if (ROMVALUE::FUTURE == update.update.secIndex)
        {
            if (pData->HasFutureProductLimits())
            {
                rc = !pData->ExceedFutureLimit(update.product, update.update, error);
            }
            else
            {
                rc = !pData->ExceedLimit(update.update, error);
            }
        }
        else
        {
            rc = !pData->ExceedLimit(update.update, error);
        }
    }

    pthread_rwlock_unlock(&m_lock);

    if (rc)
    {
        if (update.update.secIndex != ROMVALUE::FUTURE &&
            update.update.secIndex != ROMVALUE::OPTIONONFUTURE)
        {
            int rtc = m_BillingGrpLimits.ExceedLimit(update.clract, update.update, error);

            if (-1 == rtc)
            {
                rc = !(m_ClearingActSecLimits.ExceedLimit(update.clearing_sec, update.update, error));
            }
            else
            {
                rc = !rtc;
            }
        }
        else
        {
            rc = !(m_ClearingActSecLimits.ExceedLimit(update.clearing_sec, update.update, error));
        }
        
        if (false == rc)
        {
            //back out the balance update from the per trader balance
            update.update.orderChange *= -1;
            update.update.shareChange *= -1;
            update.update.dollarChange *= -1;

            if (update.product.empty() == false)
            {
                pData->UpdateFutureBalance(update.product, update.update);
            }
            else
            {
                pData->UpdateBalance(update.update);
            }
        }
    }
    return rc;
}


bool CClientProfileManagerNew::UpdateClearingActSecBalance(const std::string& key, const tBalanceUpdate& update)
{
    //if (CConfig::getInstance().ClearingLimitsEnabled() == 0)
    //{
    //    return true;
    //}
    bool rc = m_ClearingActSecLimits.UpdateBalance(key, update);
    return rc;
}

bool CClientProfileManagerNew::GetClearingActSecLimits(const std::string& ClearingActSecKey, tLimits& limits)
{
    bool rc = m_ClearingActSecLimits.GetLimits(ClearingActSecKey, limits);
    return rc;
}

void* CClientProfileManagerNew::ReloadClearingActSecLimits(void* pArg)
{
    std::string* pClearingActSec = (std::string*)pArg;
    CClientProfileManagerNew::getInstance().ReloadClearingActSecLimits(pClearingActSec);

    if (pClearingActSec)
    {
        delete pClearingActSec;
    }

    return NULL;
}

void* CClientProfileManagerNew::ReloadBillingGrpLimits(void* pArg)
{
    int* billinggrpid = (int*)pArg;
    CClientProfileManagerNew::getInstance().ReloadBillingGrpLimits(billinggrpid);
    if (billinggrpid)
    {
        delete billinggrpid;
    }
    
    return NULL;
}

void CClientProfileManagerNew::ReloadClearingActSecLimits(const std::string* key)
{
    m_ClearingActSecLimits.ReloadFromDB(key);
}

void CClientProfileManagerNew::ReloadBillingGrpLimits(const int* billinggrpid)
{
    m_BillingGrpLimits.ReloadFromDB(billinggrpid);
}

bool CClientProfileManagerNew::GetBillingGrpLimits(int billinggrpid, theLimits& limits)
{
    bool rc = m_BillingGrpLimits.GetLimits(billinggrpid, limits);
    return rc;
}

bool CClientProfileManagerNew::FindTrader(const std::string& trader)
{
    return m_TraderToTraderData.Find(trader);
}

CThreadLimits::CThreadLimits()
{
    memset(&m_limits, 0, sizeof(m_limits));
    pthread_rwlock_init(&m_lock, NULL);
}

CThreadLimits::CThreadLimits(const std::string& key, const tLimits& limits)
{
    m_limits = limits;
    m_key = key;

    pthread_rwlock_init(&m_lock, NULL);
}

CThreadLimits::~CThreadLimits()
{
    pthread_rwlock_destroy(&m_lock);
}

bool CThreadLimits::ExceedLimit(const tBalanceUpdate& BUpdate, std::string& error)
{
    bool rc = false;
    
    pthread_rwlock_wrlock(&m_lock);

    char errMsg[128];

    int newNumOrders = m_limits.NumOrders + BUpdate.orderChange;
    long newNumShares = m_limits.NumShares + BUpdate.shareChange;
    double newBalance = m_limits.Balance[BUpdate.sideIndex] + BUpdate.dollarChange;
    
    if (false == m_limits.exempted)
    {
        //MaxOrder
        if (m_limits.MaxOrders < newNumOrders)
        {
            sprintf(errMsg, "Exceeds %s's MaxOrders:%d", m_key.c_str(), m_limits.MaxOrders);
            error = errMsg;
            rc = true;
            goto JUMP;
        }

        //MaxShares
        if (BUpdate.shareChange > m_limits.MaxShares)
        {
            sprintf(errMsg, "Order's Shares Exceeds %s's Max Shares:%ld", m_key.c_str(), m_limits.MaxShares);
            error = errMsg;
            rc = true;
            goto JUMP;
        }

        //MaxDollars
        if (BUpdate.dollarChange > m_limits.MaxDollars)
        {
            sprintf(errMsg, "Order Exceeds %s's Max Dollars Allowed Per Order:%f", m_key.c_str(), m_limits.MaxDollars);
            error = errMsg;
            rc = true;
            goto JUMP;
        }

        //Balance
        //double newBalance = m_limits.Balance[BUpdate.sideIndex] + BUpdate.dollarChange;

        if (newBalance > m_limits.BuyingPower ||
            newBalance < -1 * m_limits.BuyingPower)
        {
            sprintf(errMsg, "Exceeds %s's Buying Power:%f", m_key.c_str(), m_limits.BuyingPower);
            error = errMsg;
            rc = true;
            goto JUMP;
        }

        //MaxTotalShares
        if (newNumShares > m_limits.MaxTotalShares)
        {
            sprintf(errMsg, "Exceeds %s's Max Total Shares:%ld", m_key.c_str(), m_limits.MaxTotalShares);
            error = errMsg;
            rc = true;
            goto JUMP;
        }
    }

    m_limits.Balance[BUpdate.sideIndex] = newBalance;
    m_limits.NumOrders = newNumOrders;
    m_limits.NumShares = newNumShares;
    
JUMP:    
   
    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CThreadLimits::UpdateBalance(const tBalanceUpdate& BUpdate)
{
    pthread_rwlock_wrlock(&m_lock);

    m_limits.Balance[BUpdate.sideIndex] += BUpdate.dollarChange;
    m_limits.NumOrders += BUpdate.orderChange;
    m_limits.NumShares += BUpdate.shareChange;

    pthread_rwlock_unlock(&m_lock);
}

void CThreadLimits::GetLimits(tLimits& limits)
{
    pthread_rwlock_rdlock(&m_lock);

    limits = m_limits;


    pthread_rwlock_unlock(&m_lock);
}

void CThreadLimits::Exempt(bool action)
{
    pthread_rwlock_rdlock(&m_lock);
    
    m_limits.exempted = action;
    
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadLimits::Exempted()
{
    pthread_rwlock_rdlock(&m_lock);
    
    bool rc = m_limits.exempted;
    
    pthread_rwlock_unlock(&m_lock);
    
    return rc;
}

void CThreadLimits::ImportBalance(const CThreadLimits& limits)
{
    pthread_rwlock_wrlock(&m_lock);

    const_cast<CThreadLimits&>(limits).GetBalance(m_limits);

    pthread_rwlock_unlock(&m_lock);
}

void CThreadLimits::GetBalance(tLimits& limits)
{
    pthread_rwlock_rdlock(&m_lock);

    limits.NumOrders = m_limits.NumOrders;
    limits.NumShares = m_limits.NumShares;
    for (int i = 0; i < ROMVALUE::SIDENUM; i++)
    {
        limits.Balance[i] = m_limits.Balance[i];
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadLimits::UpdateLimits(const tLimits& limits)
{
    pthread_rwlock_wrlock(&m_lock);

    m_limits.BuyingPower = limits.BuyingPower;
    m_limits.MaxDollars = limits.MaxDollars;
    m_limits.MaxOrders = limits.MaxOrders;
    m_limits.MaxShares = limits.MaxShares;
    m_limits.MaxTotalShares = limits.MaxTotalShares;

    pthread_rwlock_unlock(&m_lock);
}

CGLimits::CGLimits() : m_limitsMap(false)
{
    pthread_rwlock_init(&m_lock, NULL);
}

CGLimits::~CGLimits()
{
    pthread_rwlock_rdlock(&m_lock);
    CRomMap<CThreadLimits*> map;
    m_limitsMap.Swap(map);
    std::list<CThreadLimits*> list;
    map.GetAll(list);
    std::list<CThreadLimits*>::iterator it = list.begin();
    while (it != list.end())
    {
        delete *it;
        it++;
    }
    pthread_rwlock_unlock(&m_lock);

    pthread_rwlock_destroy(&m_lock);
}

bool CGLimits::ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    bool rc = false;

    if (m_limitsMap.Get(key, pLimits))
    {
        rc = pLimits->ExceedLimit(BUpdate, error);
    }
    else
    {
        rc = ProcessInvalidKey(key, error);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CGLimits::IsEmpty()
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = (m_limitsMap.Size() > 0)? false : true;
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CGLimits::Exempt(const std::string& key, bool action)
{
    bool rc = false;
    
    pthread_rwlock_rdlock(&m_lock);
    
    CThreadLimits* limits = NULL;
    
    if (m_limitsMap.Get(key, limits))
    {
        rc = true;
        limits->Exempt(action);
    }
    
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CGLimits::Find(const std::string& key)
{
    bool rc = false;
    
    pthread_rwlock_rdlock(&m_lock);
    
    rc = m_limitsMap.Find(key);
    
    pthread_rwlock_unlock(&m_lock);
    
    return rc;
}

bool CGLimits::Exempted(const std::string& key)
{
    bool rc = false;
    
    pthread_rwlock_rdlock(&m_lock);
    
    CThreadLimits* limits = NULL;
    
    if (m_limitsMap.Get(key, limits))
    {
        rc = limits->Exempted();
    }
    
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CGLimits::UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    if (m_limitsMap.Get(key, pLimits))
    {
        pLimits->UpdateBalance(BUpdate);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CGLimits::GetLimits(const std::string& key, tLimits& limits)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    if (m_limitsMap.Get(key, pLimits))
    {
        rc = true;
        pLimits->GetLimits(limits);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CGLimits::AddLimits(const std::string& key, const tLimits& limits)
{
    bool rc = false;

    pthread_rwlock_wrlock(&m_lock);

    if (m_limitsMap.Find(key) == false)
    {
        CThreadLimits* pLimits = new CThreadLimits(key, limits);
        rc = m_limitsMap.Add(key, pLimits);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CGLimits::ReloadFromDB(const std::string* key)
{
    if (key)
    {
        ReloadLimits(key);
    }
    else
    {
        ReloadAllLimits();
    }
}

void CGLimits::CopyOutLimitsMap(CRomMap<tLimits*>& map)
{
    pthread_rwlock_rdlock(&m_lock);

    std::list<CRomMap<CThreadLimits*>::tKeyValuePair> keyvalues;
    m_limitsMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CThreadLimits*>::tKeyValuePair>::iterator it = keyvalues.begin();

    while (it != keyvalues.end())
    {
        tLimits* pLimits = new tLimits;
        (it->value)->GetLimits(*pLimits);
        map.Add(it->key, pLimits);
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}

CGLimits& CGLimits::operator=(const CGLimits& obj)
{
    std::list<CRomMap<CThreadLimits*>::tKeyValuePair> keyvalues;
    const_cast<CGLimits&>(obj).m_limitsMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CThreadLimits*>::tKeyValuePair>::iterator it = keyvalues.begin();
    tLimits limits;

    while (it != keyvalues.end())
    {
        const_cast<CGLimits&>(obj).GetLimits(it->key, limits);
        AddLimits(it->key, limits);
        it++;
    }

    return *this;
}


void CGLimits::CopyOutBalance(CGLimits* glimits)
{
    std::list<CRomMap<CThreadLimits*>::tKeyValuePair> keyvalues;
    m_limitsMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CThreadLimits*>::tKeyValuePair>::iterator it = keyvalues.begin();

    while (it != keyvalues.end())
    {
        glimits->ImportBalance(it->key, *(it->value));
        it++;
    }
}

void CUserFutureLimits::MakeLimits(tLimits& limits, std::string& product, queue_t row)
{
    gendb_v* pVal = NULL;
    element_t colE;
    int cnt = 0;

    while (is_empty(row) == 0)
    {
        colE = pop_front(row);
        pVal = (gendb_v*)(colE->d);

        if (pVal->type_in_union != DB_NULL)
        {
            switch(cnt)
            {
            case 0:
                if (pVal->arr_len)
                {
                    product.assign((pVal->g_un).un_array, pVal->arr_len);
                }
                break;
            case 1:
                limits.MaxShares = (pVal->g_un).un_int;
                break;
            case 2:
                limits.MaxTotalShares = (pVal->g_un).un_int;
                break;
            case 3:
                limits.MaxOrders = (pVal->g_un).un_int;
                break;
            case 4:
                limits.MaxDollars = (pVal->g_un).un_double;
                break;
            case 5:
                limits.BuyingPower = (pVal->g_un).un_double;
                break;
            default:
                break;
            }
        }

        cnt++;
        free(colE);
        gendb_destroy_v(pVal);
    }
}

void CUserFutureLimits::ReloadAllLimits()
{
    queue_t results = create_queue();
    //const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    std::string sqlStr;
    char sqlstr[1024];

    sprintf(sqlstr, "select Product, MaxContracts, TotalMaxContracts, "
            "MaxOrders, MaxDollars, BuyingPower "
            "from FutureLimits where username='%s'", m_username.c_str());

    sqlStr.assign(sqlstr);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, false, {0,0}};
        std::string product;
        queue_t row = NULL;
        element_t rowE;
        CRomMap<CThreadLimits*> oldMap;
        SwapMap(oldMap);

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            MakeLimits(limits, product, row);
            AddLimits(product, limits);

            free(rowE);
            destroy_queue(row);
        }

        std::list<CRomMap<CThreadLimits*>::tKeyValuePair> keyvalues;
        oldMap.GetKeyValuePairs(keyvalues);
        std::list<CRomMap<CThreadLimits*>::tKeyValuePair>::iterator it = keyvalues.begin();

        CThreadLimits* ptLimits = NULL;

        while (it != keyvalues.end())
        {
            ptLimits = it->value;
            ImportBalance(it->key, *ptLimits);
            it++;
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
}

void CUserFutureLimits::ReloadLimits(const std::string* product)
{
    queue_t results = create_queue();
    std::string errStr;

    std::string sqlStr;
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlstr[1024];

    sprintf(sqlstr, "select product, MaxContracts, TotalMaxContracts, "
            "MaxOrders, MaxDollars, BuyingPower "
            "from FutureLimits where "
            "Product='%s' and username='%s'",
            product->c_str(), m_username.c_str());

    sqlStr.assign(sqlstr);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, false, {0,0}};
        queue_t row = NULL;
        element_t rowE;

        if (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            std::string dummy;
            MakeLimits(limits, dummy, row);

            if (UpdateLimits(*product, limits) == false)
            {
                AddLimits(*product, limits);
            }

            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
}

bool CUserFutureLimits::ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, bool exempted, std::string& error)
{
    bool rc = false;

    if (exempted)
    {
        rc = UpdateBalance(key, BUpdate);
    }
    else
    {
        rc = CGLimits::ExceedLimit(key, BUpdate, error);
    }

    return rc;
}

void CClearingActLimits::MakeLimits(tLimits& limits, std::string& key, queue_t row)
{
    gendb_v* pVal = NULL;
    element_t colE;
    int cnt = 0;

    while (is_empty(row) == 0)
    {
        colE = pop_front(row);
        pVal = (gendb_v*)(colE->d);

        if (pVal->type_in_union != DB_NULL)
        {
            switch(cnt)
            {
            case 0:
                if (pVal->arr_len)
                {
                    key.assign((pVal->g_un).un_array, pVal->arr_len);
                }
                break;
            case 1:
                if (pVal->arr_len)
                {
                    key.append("/");
                    key.append((pVal->g_un).un_array, pVal->arr_len);
                }
                break;
            case 2:
                limits.MaxShares = (pVal->g_un).un_int;
                break;
            case 3:
                limits.MaxTotalShares = (pVal->g_un).un_int;
                break;
            case 4:
                limits.MaxOrders = (pVal->g_un).un_int;
                break;
            case 5:
                limits.MaxDollars = (pVal->g_un).un_double;
                break;
            case 6:
                limits.BuyingPower = (pVal->g_un).un_double;
                break;
            default:
                break;
            }
        }

        cnt++;
        free(colE);
        gendb_destroy_v(pVal);
    }
}

void CClearingActLimits::ReloadAllLimits()
{
    queue_t results = create_queue();
    std::string errStr;

    std::string sqlStr;
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlstr[1024];

    strcpy(sqlstr, "select ClearingAcID, Type, MaxShares, TotalMaxShares, MaxOrders, MaxDollars, BuyingPower "
                "from ClearingActToLimits where active=1");
    sqlStr.assign(sqlstr);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, false, {0,0}};
        std::string key;
        queue_t row = NULL;
        element_t rowE;

        CRomMap<CThreadLimits*> oldMap;
        SwapMap(oldMap);

        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            MakeLimits(limits, key, row);

            AddLimits(key, limits);

            free(rowE);
            destroy_queue(row);
        }

        std::list<CRomMap<CThreadLimits*>::tKeyValuePair> keyvalues;
        oldMap.GetKeyValuePairs(keyvalues);
        std::list<CRomMap<CThreadLimits*>::tKeyValuePair>::iterator it = keyvalues.begin();

        CThreadLimits* ptLimits = NULL;

        while (it != keyvalues.end())
        {
            ptLimits = it->value;;
            ImportBalance(it->key, *ptLimits);
            it++;
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
}

void CClearingActLimits::ReloadLimits(const std::string* key)
{
    queue_t results = create_queue();
    //const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    //char sqlStr[128];

    std::string sqlStr;
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlstr[1024];

    int index = key->find('/');

    std::string clearing = key->substr(0, index);
    std::string type = key->substr(index + 1);

    sprintf(sqlstr, "select ClearingAcID, Type, MaxShares, TotalMaxShares, MaxOrders, MaxDollars, BuyingPower "
            "from ClearingActToLimits where active=1 and ClearingAcID='%s' and Type='%s'",
            clearing.c_str(), type.c_str());
    sqlStr.assign(sqlstr);
    
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, false, {0,0}};
        //std::string Key;
        queue_t row = NULL;
        element_t rowE;

        if (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            std::string dummy;
            MakeLimits(limits, dummy, row);

            if (UpdateLimits(*key, limits) == false)
            {
                AddLimits(*key, limits);
            }

            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
}



bool CGLimits::UpdateLimits(const std::string& key, const tLimits& limits)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;

    if (m_limitsMap.Get(key, pLimits))
    {
        pLimits->UpdateLimits(limits);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}


bool CUserFutureLimits::ProcessInvalidKey(const std::string& key, std::string& error)
{
    error.assign(m_username);
    error.append(" is not allowed to trade future product:");
    error.append(key);
    return true;
}

void CUserFutureLimits::ReLoad(const std::string* product)
{
    if (product)
    {
        ReloadLimits(product);
    }
    else
    {
        ReloadAllLimits();
    }
}

void CGLimits::SwapMap(CRomMap<CThreadLimits*>& map)
{
    m_limitsMap.Swap(map);
}

void CGLimits::ImportBalance(const std::string& key, const CThreadLimits& limits)
{
    CThreadLimits* pLimits = NULL;

    if (m_limitsMap.Get(key, pLimits))
    {
        pLimits->ImportBalance(limits);
    }
}


CBillingGrpLimits::CBillingGrpLimits()
{
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
    ReloadFromDB(NULL);
}

CBillingGrpLimits::~CBillingGrpLimits()
{
    pthread_spin_lock(&m_lock);
    
    std::map<int, theLimits*>::iterator it = m_BillingGrpIdToLimits.begin();
    theLimits* pLimits = NULL;
    
    while (it != m_BillingGrpIdToLimits.end())
    {
        pLimits = it->second;
        if (pLimits->name)
        {
            delete pLimits->name;
        }
        delete pLimits;
        it++;
    }
    
    pthread_spin_unlock(&m_lock);
    
    pthread_spin_destroy(&m_lock);
    
}
    
int CBillingGrpLimits::ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error)
{
    double dollarChange = BUpdate.dollarChange;
    char errMsg[128];
    std::string secStr;
    CUtility::GetSecTypeStr(BUpdate.secIndex, secStr);
    
    pthread_spin_lock(&m_lock);
    
    std::map<std::string, theLimits*>::iterator it = m_ClrToLimits.find(key);
    theLimits* pLimits = NULL;
    int rc = 0;
    
    if (it != m_ClrToLimits.end())
    {
        pLimits = it->second;
        int newNumOrders = (pLimits->limits)[BUpdate.secIndex].NumOrders + BUpdate.orderChange;
        long newNumShares = (pLimits->limits)[BUpdate.secIndex].NumShares + BUpdate.shareChange;
        //double newBalance = 0;
        double newBalance = (pLimits->limits)[BUpdate.secIndex].Balance[BUpdate.sideIndex] + dollarChange;

        if (false == pLimits->exempted)
        {
            //MaxOrder
            if ((pLimits->limits)[BUpdate.secIndex].MaxOrders < newNumOrders)
            {
                sprintf(errMsg, "Exceeds BillingGroup(%s)'s %s MaxOrders:%d", pLimits->name->c_str(), secStr.c_str(), (pLimits->limits)[BUpdate.secIndex].MaxOrders);
                error = errMsg;
                rc = 1;
                goto JUMP;
            }

            //MaxShares
            if (BUpdate.shareChange > (pLimits->limits)[BUpdate.secIndex].MaxShares)
            {
                sprintf(errMsg, "Order's Shares Exceeds BillingGroup(%s)'s %s Max Shares:%ld", pLimits->name->c_str(), secStr.c_str(), (pLimits->limits)[BUpdate.secIndex].MaxShares);
                error = errMsg;
                rc = 1;
                goto JUMP;
            }

            //MaxDollars
            if (dollarChange > (pLimits->limits)[BUpdate.secIndex].MaxDollars)
            {
                sprintf(errMsg, "Order Exceeds BillingGroup(%s)'s %s Max Dollars Allowed Per Order:%f", pLimits->name->c_str(), secStr.c_str(), (pLimits->limits)[BUpdate.secIndex].MaxDollars);
                error = errMsg;
                rc = 1;
                goto JUMP;
            }

            //Balance
            //newBalance = (pLimits->limits)[BUpdate.secIndex].Balance[BUpdate.sideIndex] + dollarChange;

            if (newBalance > (pLimits->limits)[BUpdate.secIndex].BuyingPower ||
                newBalance < -1 * (pLimits->limits)[BUpdate.secIndex].BuyingPower)
            {
                sprintf(errMsg, "Exceeds BillingGroup(%s)'s %s Buying Power:%f", pLimits->name->c_str(), secStr.c_str(), (pLimits->limits)[BUpdate.secIndex].BuyingPower);
                error = errMsg;
                rc = 1;
                goto JUMP;
            }

            //MaxTotalShares
            if (newNumShares > (pLimits->limits)[BUpdate.secIndex].MaxTotalShares)
            {
                sprintf(errMsg, "Exceeds BillingGroup(%s)'s %s Max Total Shares:%ld", pLimits->name->c_str(), secStr.c_str(), (pLimits->limits)[BUpdate.secIndex].MaxTotalShares);
                error = errMsg;
                rc = 1;
                goto JUMP;
            }
        }

        (pLimits->limits)[BUpdate.secIndex].Balance[BUpdate.sideIndex] = newBalance;
        (pLimits->limits)[BUpdate.secIndex].NumOrders = newNumOrders;
        (pLimits->limits)[BUpdate.secIndex].NumShares = newNumShares;
    }
    else
    {
        rc = -1; 
    }
JUMP:
    pthread_spin_unlock(&m_lock);
    return rc;
}

bool CBillingGrpLimits::UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
            
    std::map<std::string, theLimits*>::iterator it = m_ClrToLimits.find(key);
        
    if (it != m_ClrToLimits.end())
    {
        tLimits& limits = (it->second)->limits[BUpdate.secIndex];
        limits.Balance[BUpdate.sideIndex] += BUpdate.dollarChange;
        limits.NumOrders += BUpdate.orderChange;
        limits.NumShares += BUpdate.shareChange;
        rc = true;
    }
       
    pthread_spin_unlock(&m_lock);
    
    
    return rc;
}

bool CBillingGrpLimits::Exempt(int bgrpid, bool action)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
    
    std::map<int, theLimits*>::iterator it = m_BillingGrpIdToLimits.find(bgrpid);
    if (it != m_BillingGrpIdToLimits.end())
    {
        theLimits* limits = it->second;
        limits->exempted = action;
        rc = true;
    }
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CBillingGrpLimits::GetLimits(int billinggrpid, int secIndex, tLimits& limits)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
    
    std::map<int, theLimits*>::iterator it = m_BillingGrpIdToLimits.find(billinggrpid);
        
    if (it != m_BillingGrpIdToLimits.end())
    {
        limits = (it->second)->limits[secIndex];
        rc = true;
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CBillingGrpLimits::GetLimits(int billinggrpid, theLimits& limits)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
    
    std::map<int, theLimits*>::iterator it = m_BillingGrpIdToLimits.find(billinggrpid);
    theLimits* pLimits = NULL;
    
    if (it != m_BillingGrpIdToLimits.end())
    {
        pLimits = it->second;
        limits.id = pLimits->id;
        limits.exempted = pLimits->exempted;
        rc = true;
        for (int i = 0; i < ROMVALUE::SECNUM; i++)
        {
            limits.limits[i] = pLimits->limits[i];
        }
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CBillingGrpLimits::LoadClrBillingGrpId(std::map<std::string, int>& billinggrptoid)
{
    bool rc = true;
    queue_t results = create_sized_queue(256);
    std::string sqlStr;
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlstr[1024];
    
    strcpy(sqlstr, "SELECT clearingAccount, groupID FROM ClearingActBillingGroupMap");
    sqlStr.assign(sqlstr);
    
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    std::string errStr;
    
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            std::string sclr;
            
            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                    case 0:
                        if(pVal->type_in_union != DB_NULL) 
                        {
                            sclr.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 1:
                        if(pVal->type_in_union != DB_NULL) 
                        {
                            billinggrptoid.insert(std::map<std::string, int>::value_type(sclr, (pVal->g_un).un_int));
                        }
                        break;
                    default:
                        break;
                }

                cnt++;
                free_element(row, colE);
                gendb_destroy_v(pVal);
            }

            free_element(results, rowE);
            destroy_queue(row);
        }
    }
    else
    {
        rc = false;
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
    destroy_queue(results);  
    return rc;
}

void CBillingGrpLimits::ReloadFromDB(const int* billinggrpid)
{
    std::map<std::string, int> clrtobillinggrpid;
    LoadClrBillingGrpId(clrtobillinggrpid);
    
    queue_t results = create_sized_queue(256);
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlstr[1024];
    
    if (billinggrpid)
    {
        sprintf(sqlstr, "select A.*, B.groupname from BillingLimits A,"
                        "billinggroup B where A.GroupID=%d AND A.groupid = B.groupid", 
                        *billinggrpid);
    }
    else
    {
        strcpy(sqlstr, "select A.*, B.groupname from BillingLimits A," 
                       "billinggroup B WHERE A.groupid = B.groupid");
    }
    
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    std::string errStr;
    std::string sqlStr = sqlstr;
    
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        std::set<int> blgset;
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        pthread_spin_lock(&m_lock);
        
        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            theLimits* pLimits = NULL;
            bool newgrpid = false;
            
            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                case 0:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        std::map<int, theLimits*>::iterator it = m_BillingGrpIdToLimits.find((pVal->g_un).un_int);
                        if (it == m_BillingGrpIdToLimits.end())
                        {
                            pLimits = new theLimits;
                            memset(pLimits, 0, sizeof(theLimits));
                            pLimits->id = (pVal->g_un).un_int;
                            m_BillingGrpIdToLimits.insert(std::map<int, theLimits*>::value_type(pLimits->id, pLimits));
                            newgrpid = true;
                        }
                        else
                        {
                            pLimits = it->second;
                        }
                        blgset.insert((pVal->g_un).un_int);
                    }
                    break;
                case 1:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::EQUITY].MaxShares = (pVal->g_un).un_int;
                    }
                    break;
                case 2:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTION].MaxShares = (pVal->g_un).un_int;
                    }
                    break;
                case 3:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::FUTURE].MaxShares = (pVal->g_un).un_int;
                    }
                    break;
                case 4:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTIONONFUTURE].MaxShares = (pVal->g_un).un_int;
                    }
                    break;
                case 5:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::EQUITY].MaxDollars = (pVal->g_un).un_double;
                    }
                    break;
                case 6:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTION].MaxDollars = (pVal->g_un).un_double;
                    }
                    break; 
                case 7:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::FUTURE].MaxDollars = (pVal->g_un).un_double;
                    }
                    break; 
                case 8:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTIONONFUTURE].MaxDollars = (pVal->g_un).un_double;
                    }
                    break; 
                case 9:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::EQUITY].MaxOrders = (pVal->g_un).un_int;
                    }
                    break;
                case 10:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTION].MaxOrders = (pVal->g_un).un_int;
                    }
                    break;
                case 11:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::FUTURE].MaxOrders = (pVal->g_un).un_int;
                    }
                    break;
                case 12:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTIONONFUTURE].MaxOrders = (pVal->g_un).un_int;
                    }
                case 13:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::EQUITY].MaxTotalShares = (pVal->g_un).un_int;
                    }
                    break;
                case 14:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTION].MaxTotalShares = (pVal->g_un).un_int;
                    }
                    break;
                case 15:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::FUTURE].MaxTotalShares = (pVal->g_un).un_int;
                    }
                    break;
                case 16:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTIONONFUTURE].MaxTotalShares = (pVal->g_un).un_int;
                    }
                    break;
                case 17:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::EQUITY].BuyingPower = (pVal->g_un).un_double;
                    }
                    break; 
                case 18:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTION].BuyingPower = (pVal->g_un).un_double;
                    }
                    break; 
                case 19:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::FUTURE].BuyingPower = (pVal->g_un).un_double;
                    }
                    break; 
                case 20:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        pLimits->limits[ROMVALUE::OPTIONONFUTURE].BuyingPower = (pVal->g_un).un_double;
                    }
                    break;
                case 21:
                    if (pVal->type_in_union != DB_NULL &&
                        pVal->arr_len) 
                    {
                        pLimits->name = new std::string;
                        pLimits->name->assign((pVal->g_un).un_array, pVal->arr_len);
                    }
                    break;
                default:
                    break;
                }
                
                cnt++;
                free_element(row, colE);
                gendb_destroy_v(pVal);
            }
            
            free_element(results, rowE);
            destroy_queue(row);
            
            if (newgrpid)
            {
                std::map<std::string, int>::iterator it = clrtobillinggrpid.begin();
                while (it != clrtobillinggrpid.end())
                {
                    if (it->second == pLimits->id)
                    {
                        m_ClrToLimits.insert(std::map<std::string, theLimits*>::value_type(it->first, pLimits));
                    }
                    it++;
                }
            }
        }
        
        //check any billinglimits have been removed
        if (NULL == billinggrpid)
        {
            std::map<int, theLimits*>::iterator itt = m_BillingGrpIdToLimits.begin();

            while (itt != m_BillingGrpIdToLimits.end())
            {
                if (blgset.find(itt->first) == blgset.end())
                {
                    delete itt->second;

                    std::map<int, theLimits*>::iterator iitt = itt;
                    itt++;
                    m_BillingGrpIdToLimits.erase(iitt);
                    std::map<std::string, int>::iterator ittt = clrtobillinggrpid.begin();
                    while (ittt != clrtobillinggrpid.end())
                    {
                        if (ittt->second == itt->first)
                        {
                            m_ClrToLimits.erase(ittt->first);
                        }
                        ittt++;
                    }
                }
                else
                {
                    itt++;
                }
            }
        }
        
        pthread_spin_unlock(&m_lock);
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }
}


bool CClientProfileManagerNew::loadClientHB(std::map<std::string, tHB>& hbMap, const std::string& trader, std::string& errStr)
{
    bool rc = true;
    char sqlstr[256];
    std::string sqlStr;
    //const tDBLogin& db = CConfig::getInstance().db();
    
    if (trader.empty())
    {
        strcpy(sqlstr, 
                "select A.username, B.interval, B.allowed_misses from OMuser A, OMuserHeartBeat B "
                "where A.OMuserID=B.OMuserID");
    }
    else
    {
        sprintf(sqlstr, 
                "select A.username, B.interval, B.allowed_misses from OMuser A, OMuserHeartBeat B "
                "where A.OMuserID=B.OMuserID and A.username='%s'", trader.c_str());
    }
    
    sqlStr.assign(sqlstr);
    
    queue_t results = create_queue();
    
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    
    if (executor->Execute(sqlstr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        element_t rowE;
        std::string username;
        
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            
            gendb_v* pVal = NULL;
            element_t colE;
            int cnt = 0;
            tHB hb = {false, 30, 3};
            
            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                
                switch(cnt)
                {
                    case 0:
                        if (pVal->arr_len)
                        {
                            username.assign((pVal->g_un).un_array, pVal->arr_len);
                            hb.enabled = true;
                        } 
                        break;
                    case 1:
                        hb.interval = (pVal->g_un).un_int;
                        break;
                    case 2:
                        hb.allowedmisses = (pVal->g_un).un_int;
                        break;
                }
                
                if (username.empty() != true)
                {
                    hbMap.insert(std::map<std::string, tHB>::value_type(username, hb));
                }
                cnt++;

                free(colE);
                gendb_destroy_v(pVal);
            }
    
            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        rc = false;
    }
    
    destroy_queue(results);
    
    return rc;
}

bool CClientProfileManagerNew::GetClientHB(tHB& hb, const std::string& clientname)
{
    bool rc = false;
    std::map<std::string, tHB> hbmap;
    std::string error;
    
    loadClientHB(hbmap, clientname, error);
    std::map<std::string, tHB>::iterator it = hbmap.begin();
    if (it != hbmap.end())
    {
        hb = it->second;
        rc = true;
    }
    
    return rc;
}

bool CClientProfileManagerNew::AllowedToReplaceCoveredSpreadOrder(tKeyedBalanceUpdate& update, long bodyshares, const tCoveredSpread* corder, std::string& error)
{
    tBalanceUpdate update_hold = update.update;
    
    update.update.orderChange = 0;
    update.update.shareChange = update_hold.shareChange * corder->e_ratio;
    update.update.sideIndex = corder->e_sideIndex;
    update.update.secIndex = ROMVALUE::EQUITY;
    update.update.dollarChange = update_hold.shareChange * corder->e_dollars;

    bool rc = AllowedToReplaceOrder(update, error);
    if (rc)
    {

        update.update.dollarChange = corder->o_delt_dollars;
        update.update.shareChange = update_hold.shareChange * corder->o_ratio;
        update.update.sideIndex = corder->o_sideIndex;
        update.update.secIndex = ROMVALUE::OPTION;
        
        rc = AllowedToReplaceOrder(update, error);
        if (false == rc)
        {
            update.update.shareChange *= -1;
            update.update.dollarChange *= -1;
            UpdateBalance(update);
        }
    }
    update.update = update_hold;
    return rc;
}

void CClientProfileManagerNew::UpdateBalanceCoveredSpread(const tKeyedBalanceUpdate& update, tCoveredSpread* corder, bool bReplaceReject)
{
    tKeyedBalanceUpdate l_update = update;
    l_update.update.shareChange = update.update.shareChange * corder->e_ratio;
    l_update.update.sideIndex = corder->e_sideIndex;
    l_update.update.secIndex = ROMVALUE::EQUITY;
    l_update.update.dollarChange = update.update.shareChange * corder->e_dollars;
    UpdateBalance(l_update);

    l_update.update.shareChange = update.update.shareChange * corder->o_ratio;
    l_update.update.sideIndex = corder->o_sideIndex;
    l_update.update.secIndex = ROMVALUE::OPTION;
    
    if (bReplaceReject) 
    {
        l_update.update.dollarChange = -1 * corder->o_delt_dollars;
        corder->SetODeltDollars(0);
    }
    else
    {
        l_update.update.dollarChange = corder->o_dollars * update.update.shareChange;
    }
    
    UpdateBalance(l_update);
}

bool CThreadTraderData::ExceedLimit_CoveredSpread(CRomOrderNew& order, std::vector<tBalanceUpdate*>& balances, std::string& error)
{
    bool rc = false;
    tBalanceUpdate* pOBU = new tBalanceUpdate;
    tBalanceUpdate* pEBU = new tBalanceUpdate;
    const tCoveredSpread* pCoveredSpread = order.CoveredSpread();

    pOBU->secIndex = ROMVALUE::OPTION;
    pOBU->sideIndex = pCoveredSpread->o_sideIndex;
    pOBU->orderChange = 1;
    pOBU->shareChange = order.Shares() * pCoveredSpread->o_ratio;
    pOBU->dollarChange = order.Shares() * pCoveredSpread->o_dollars;

    rc = ExceedLimit(*pOBU, error);
    if (false == rc)
    {
        //tBalanceUpdate* pEBU = new tBalanceUpdate;
        pEBU->secIndex = ROMVALUE::EQUITY;
        pEBU->sideIndex = pCoveredSpread->e_sideIndex;
        pEBU->orderChange = 1;
        pEBU->shareChange = order.Shares() * pCoveredSpread->e_ratio;
        pEBU->dollarChange = order.Shares() * pCoveredSpread->e_dollars;
        rc = ExceedLimit(*pEBU, error);
        if (rc)
        {
            delete pEBU;
            //back out the option contribution
            pOBU->orderChange *= -1;
            pOBU->shareChange *= -1;
            pOBU->dollarChange *= -1;
            UpdateBalance(*pOBU);
        }
    }
    else
    {
        delete pOBU;
    }
    
    if (false == rc)
    {
        balances.push_back(pOBU);
        balances.push_back(pEBU);
    }

    return rc;
}