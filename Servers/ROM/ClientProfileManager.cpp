#include "ClientProfileManager.h"
#include "Config.h"
#include "ClientsManager.h"
#include "debug_logger.h"

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
             //IsOtherTrader(tradeFor.c_str()) == false))
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

    pData->SetEquityLimits(m_EquityLimits);
    pData->SetOptionLimits(m_OptionLimits);
    pData->SetFutureLimits(m_FutureLimits);
    pData->SetCurrencyLimits(m_CurrencyLimits);
}

void CTraderData::GetBalance(tBalanceUpdate& update)
{
    tLimits* pLimits = GetLimits(update.secType);
    //strcpy(update.trader, Trader().c_str());
    //update.sideSign = 1;
    update.dollarChange = pLimits->Balance;
    update.shareChange = pLimits->NumShares;
    update.orderChange = pLimits->NumOrders;
    //update.dollarChange = Balance();
    //update.shareChange = NumShares();
    //update.orderChange = NumOrders();
}

bool CTraderData::IsSubTrader(const char* trader)
{
    tStringSet::iterator it = m_SubTraderSet.find("A");
    /*if (it != m_SubTraderSet.end())
    {
    	return true;
    }

    it = m_SubTraderSet.find("N");
    if (it != m_SubTraderSet.end())
    {
    	return false;
    }*/

    it = m_SubTraderSet.find(trader);
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
    tLimits* pLimits = GetLimits(update.secType);
    pLimits->Balance += update.dollarChange;
    pLimits->NumOrders += update.orderChange;
    pLimits->NumShares += update.shareChange;
}

void CTraderData::SetBalance(const tBalanceUpdate& update)
{
    tLimits* pLimits = GetLimits(update.secType);
    pLimits->Balance = update.dollarChange;
    pLimits->NumOrders = update.orderChange;
    pLimits->NumShares = update.shareChange;
}


void CTraderData::LoadSubTraders(std::string traders)
{
    std::string::size_type ps = traders.find(',');
    std::string subtrader;

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
    tLimits* pLimits = GetLimits(theChange.secType);
    double dollarChange = theChange.dollarChange;
    char errMsg[128];
    std::string secStr;
    CUtility::GetSecTypeStr(theChange.secType, secStr);

    if (m_Exempt)
    {
        pLimits->Balance += dollarChange;
        pLimits->NumOrders += theChange.orderChange;
        pLimits->NumShares += theChange.shareChange;
        return false;
    }

    int newNumOrders = pLimits->NumOrders + theChange.orderChange;
    long newNumShares = pLimits->NumShares + theChange.shareChange;

    //MaxOrder
    if (pLimits->MaxOrders < newNumOrders)
    {
        sprintf(errMsg, "Exceeds %s's %s MaxOrders:%d", m_Trader.c_str(), secStr.c_str(), pLimits->MaxOrders);
        error = errMsg;
        return true;
    }

    //MaxShares
    if (theChange.shareChange > pLimits->MaxShares)
    {
        sprintf(errMsg, "Order's Shares Exceeds %s's %s Max Shares:%ld", m_Trader.c_str(), secStr.c_str(), pLimits->MaxShares);
        error = errMsg;
        return true;
    }

    //MaxDollars
    if (dollarChange > pLimits->MaxDollars)
    {
        sprintf(errMsg, "Order Exceeds %s's %s Max Dollars Allowed Per Order:%f", m_Trader.c_str(), secStr.c_str(), pLimits->MaxDollars);
        error = errMsg;
        return true;
    }

    //Balance
    double newBalance = pLimits->Balance + dollarChange;

    if (newBalance > pLimits->BuyingPower ||
            newBalance < -1 * pLimits->BuyingPower)
    {
        sprintf(errMsg, "Exceeds %s's %s Buying Power:%f", m_Trader.c_str(), secStr.c_str(), pLimits->BuyingPower);
        error = errMsg;
        return true;
    }

    //MaxTotalShares
    if (newNumShares > pLimits->MaxTotalShares)
    {
        sprintf(errMsg, "Exceeds %s's %s Max Total Shares:%ld", m_Trader.c_str(), secStr.c_str(), pLimits->MaxTotalShares);
        error = errMsg;
        return true;
    }

    pLimits->Balance = newBalance;
    pLimits->NumOrders = newNumOrders;
    pLimits->NumShares = newNumShares;

    return false;
}


CThreadTraderData::CThreadTraderData(CTraderData* pData)
{
    pthread_rwlock_init(&m_lock, NULL);
    pData->Get(&m_data);
}

CThreadTraderData::~CThreadTraderData()
{
    pthread_rwlock_destroy(&m_lock);
}

void CThreadTraderData::GetDropInfo(tDropInfo& dropInfo)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data.GetDropInfo(dropInfo);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::ExceedLimit(const tBalanceUpdate& change, std::string& error)
{
    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data.ExceedLimit(change, error);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CThreadTraderData::ExceedLimit(const CRomOrder& order, tBalanceUpdate& BUpdate, std::string& error)
{
    BUpdate.orderChange = 1;

    BUpdate.shareChange = order.Shares();
    BUpdate.secType = order.SecType();
    BUpdate.dollarChange = order.SideSign() * BUpdate.shareChange * order.Price() * order.Multiplier();

    pthread_rwlock_wrlock(&m_lock);
    bool rc = m_data.ExceedLimit(BUpdate, error);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CThreadTraderData::ExceedLimit_Cplx(CRomOrder& order, const vector<CCsv*>* pLegs, tBalanceUpdate& BUpdate, std::string& error)
{
    CCsv* pChild = NULL;
    bool rc = false;
    int ratio = 0, total_ratio = 0;

    pChild = (*pLegs)[0];
    char sectype = ROMVALUE::SECTYPE_EQUITY;



    if (order.MixedSpread())
    {
        for (size_t i = 0; i < pLegs->size(); i++)
        {
            pChild = (*pLegs)[i];

            if (pChild->GetAt(ROMFIELDS::CSV_SHARES, ratio))
            {
                if (pChild->GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype) &&
                    ROMVALUE::SECTYPE_OPTION == sectype)
                {
                    ratio *= 100;
                }
                total_ratio += ratio;
            }
        }

        int roundUp = 0;
        if (total_ratio % 100)
        {
            roundUp = 1;
        }
        total_ratio = total_ratio / 100 + roundUp;
    }
    else
    {
        for (size_t i = 0; i < pLegs->size(); i++)
        {
            pChild = (*pLegs)[i];
            if (pChild->GetAt(ROMFIELDS::CSV_SHARES, ratio))
            {
                total_ratio += ratio;
            }
        }
    }
    //total cash contribution, assuming that currency rate is one
    BUpdate.secType = order.SecType();
    BUpdate.orderChange = 1;
    BUpdate.shareChange = total_ratio * order.Shares();
    BUpdate.dollarChange = order.SideSign() * order.Multiplier() * order.Shares() * order.Price();

    rc = ExceedLimit(BUpdate, error);
    order.SetRatio(total_ratio);

    return rc;
}

void CThreadTraderData::UpdateBalance(const tBalanceUpdate& update)
{
    pthread_rwlock_wrlock(&m_lock);
    m_data.UpdateBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::SetBalance(const tBalanceUpdate& update)
{
    pthread_rwlock_wrlock(&m_lock);
    m_data.SetBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetBalance(tBalanceUpdate& update)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data.GetBalance(update);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::CanTradeFor(const std::string& tradeFor)
{
    bool rt = false;
    pthread_rwlock_rdlock(&m_lock);
    rt = m_data.CanTradeFor(tradeFor);
    pthread_rwlock_unlock(&m_lock);
    return rt;
}

void CThreadTraderData::ExemptTrader(bool exempt)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data.SetExempt(exempt);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadTraderData::Exempted()
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_data.Exempt();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CThreadTraderData::GetPassword(std::string& pwd)
{
    pthread_rwlock_rdlock(&m_lock);
    pwd = m_data.Password();
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::UpdatePassword(const std::string& pwd)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data.SetPassword(pwd);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadTraderData::GetTraderData(CTraderData* pData)
{
    pthread_rwlock_rdlock(&m_lock);
    m_data.Get(pData);
    pthread_rwlock_unlock(&m_lock);
}

CClientProfileManager::CClientProfileManager() : m_TraderToTraderData(true)
{
    pthread_rwlock_init(&m_lock, NULL);
    LoadProfilesFromDB();
    m_ClearingActSecLimits.ReloadFromDB();
}

CClientProfileManager::~CClientProfileManager()
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

bool CClientProfileManager::GetPassword(const std::string& login, std::string& password)
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

bool CClientProfileManager::UpdatePassword(const std::string& login, std::string& password)
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

bool CClientProfileManager::ValidateLogin(const std::string& login, const std::string& password, std::string& err)
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

void CClientProfileManager::GetLoginList(ROMTYPEDEF::tLoginList& rLoginList)
{
    pthread_rwlock_rdlock(&m_lock);

    std::list<CRomMap<std::string>::tKeyValuePair> keyvalues;
    m_LoginToPassword.GetKeyValuePairs(keyvalues);
    ROMTYPEDEF::tLoginStruct login;
    std::list<CRomMap<std::string>::tKeyValuePair>::iterator it = keyvalues.begin();
    while (it != keyvalues.end())
    {
        login.login = (*it).key;
        login.password = (*it).value;
        rLoginList.push_back(login);
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}

bool CClientProfileManager::GetTraderData(const std::string& trader, CTraderData* pData)
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

bool CClientProfileManager::GetDropInfo(const std::string& trader, tDropInfo& dropInfo)
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

bool CClientProfileManager::ExemptTrader(std::string trader, bool action)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadTraderData* ptData = NULL;
    if (m_TraderToTraderData.Get(trader, ptData))
    {
        ptData->ExemptTrader(action);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClientProfileManager::UpdateBalance(const std::string& trader, const tBalanceUpdate& update)
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

void* CClientProfileManager::ReloadProfileFromDB(void* pArg)
{
    std::string* pTrader = (std::string*)pArg;

    if (pTrader)
    {
        CClientProfileManager::getInstance().LoadProfileFromDB(pTrader);
        delete pTrader;
    }
    else
    {
        CClientProfileManager::getInstance().LoadProfilesFromDB(true);
    }
    return NULL;
}

/*bool CClientProfileManager::LoadProfileFromDB()
{
  bool rc = true;
  queue_t results = create_queue();
  const char* pUser = (CConfig::getInstance().dbLogin()).c_str();
  const char* pPwd = (CConfig::getInstance().dbPassword()).c_str();
  const char* pServer = (CConfig::getInstance().dbServer()).c_str();

  //pthread_rwlock_wrlock(&m_lock);

  if (CUtility::DbExec("rom.dbo.ROMGetROMusers", pServer, pUser, pPwd, &results))
  {
    queue_t row = NULL;
    gendb_v* pVal = NULL;
    element_t rowE, colE;
    CRomMap<CThreadTraderData*> tempTraderToTraderData(false);

    while (is_empty(results) == 0)
    {
      int cnt = 0;
      rowE = pop_front(results);
      row = (queue_t)(rowE->d);
      CTraderData TraderData;

      while (is_empty(row) == 0)
      {
        colE = pop_front(row);
        pVal = (gendb_v*)(colE->d);

        switch(cnt)
        {
        case 0:
          TraderData.SetTrader((pVal->g_un).un_array);
          break;
        case 1:
          TraderData.SetPassword((pVal->g_un).un_array);
          break;
        case 5:
          TraderData.SetBuyingPower((pVal->g_un).un_double);
          break;
        case 6:
          TraderData.SetMaxShares((pVal->g_un).un_int);
          break;
        case 7:
          TraderData.SetMaxDollars((pVal->g_un).un_double);
          break;
        case 8:
          TraderData.SetMaxOrders((pVal->g_un).un_int);
          break;
        case 10:
          TraderData.SetOtherTraders((pVal->g_un).un_array);
          break;
        case 11:
          if ((pVal->g_un).un_int == 0)
          {
				    TraderData.SetDiscretionaryAbility(false);
          }
          else
          {
            TraderData.SetDiscretionaryAbility(true);
          }
          break;
        case 12:
          if ((pVal->g_un).un_int == 0)
          {
				    TraderData.SetPrincipalAbility(false);
          }
          else
          {
            TraderData.SetPrincipalAbility(true);
          }
          break;
        case 13:
          TraderData.SetGroup((pVal->g_un).un_array);
          break;
        case 14:
          TraderData.SetMaxTotalShares((pVal->g_un).un_int);
          break;
        case 15:
          TraderData.SetMaxOptionContracts((pVal->g_un).un_int);
          break;
        case 16:
          TraderData.SetMaxFutContracts((pVal->g_un).un_int);
          break;
        case 17:
          TraderData.SetReportingFirm((pVal->g_un).un_array);;
          break;
        case 18:
          TraderData.SetReportingType((pVal->g_un).un_int);
          break;
        case 19:
          TraderData.SetRoninMember((pVal->g_un).un_int);
          break;
        case 20:
          TraderData.LoadSubTraders((pVal->g_un).un_array);
          break;
        default:
          break;
        }

        cnt++;
      }

      destroy_queue(row);
      CThreadTraderData* ptData = new CThreadTraderData(&TraderData);
      tempTraderToTraderData.Add(TraderData.Trader(), ptData);
      m_LoginToPassword.Add(TraderData.Trader(), TraderData.Password());
    }

    destroy_queue(results);

    pthread_rwlock_wrlock(&m_lock);

    //Need to add handling newly added traders later!!!
    m_TraderToTraderData.Swap(tempTraderToTraderData);

    std::list<CThreadTraderData*> oldData;

    tempTraderToTraderData.GetAll(oldData);

    std::list<CThreadTraderData*>::iterator it = oldData.begin();
	  CThreadTraderData *ptTrader = NULL;
  	tBalanceUpdate tbdata;

	  //resume current balance
    while (it != oldData.end())
	  {
		  ptTrader = *it;
      ptTrader->GetBalance(tbdata);
      CThreadTraderData* ptData = NULL;
      if (m_TraderToTraderData.Get(ptTrader->Trader(), ptData))
      {
        ptData->SetBalance(tbdata);
      }
      it++;
		  delete ptTrader;
	  }
  }
  else
  {
    rc = false;
  }

  pthread_rwlock_unlock(&m_lock);

  return rc;
}*/

bool CClientProfileManager::LoadProfileFromDB(const std::string* pTrader)
{
    bool rc = true;
    queue_t results = create_queue();
    const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    char sqlStr[128];

    if (pTrader)
    {
        sprintf(sqlStr, "%s.dbo.ROMGetROMusersNew '%s'", db.database.c_str(), pTrader->c_str());
    }

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    //if (CUtility::DbExec(sqlStr, db.dbserver.c_str(), db.user.c_str(), db.password.c_str(), &results))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        CRomMap<CThreadTraderData*> tempTraderToTraderData(false);

        if (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            CTraderData TraderData;
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
                            TraderData.SetTrader(strvalue);
                        }
                        break;
                    case 1:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetPassword(strvalue);
                        }
                        break;
                    case 4:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetReportingFirm(strvalue);
                        }
                        break;
                    case 5:
                        TraderData.SetReportingType((pVal->g_un).un_int);
                        break;
                    case 6:
                        TraderData.SetRoninMember((pVal->g_un).un_int);
                        break;
                    case 7:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.LoadSubTraders(strvalue);
                        }
                        break;
                    case 9:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetOtherTraders(strvalue);
                        }
                        break;
                    case 10:
                        if ((pVal->g_un).un_int == 0)
                        {
                            TraderData.SetDiscretionaryAbility(false);
                        }
                        else
                        {
                            TraderData.SetDiscretionaryAbility(true);
                        }
                        break;
                    case 11:
                        if ((pVal->g_un).un_int == 0)
                        {
                            TraderData.SetPrincipalAbility(false);
                        }
                        else
                        {
                            TraderData.SetPrincipalAbility(true);
                        }
                        break;
                    case 12:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetGroup(strvalue);
                        }
                        break;
                    case 13:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 14:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 15:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 16:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 17:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 18:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 19:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 20:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 21:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 22:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 23:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 24:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 25:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 26:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 27:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 28:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 29:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 30:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 31:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 32:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    default:
                        break;
                    }
                }

                cnt++;
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
            CThreadTraderData* ptData = new CThreadTraderData(&TraderData);

            if (m_TraderToTraderData.Find(TraderData.Trader()) == false)
            {
                m_TraderToTraderData.Add(TraderData.Trader(), ptData);

                CClientsManager::getInstance().AddNewClient(TraderData.Trader(), TraderData.Password());
                /*
                if (pAsyncServer)
                {
                    std::string password;
                    ptData->GetPassword(password);
                    struct init_args* pArgs = (struct init_args*)calloc(1, sizeof(struct init_args));
                    pArgs->name_len = ptData->Trader().size();
                    pArgs->name = (char*)calloc(1, pArgs->name_len + 1);
                    strcpy(pArgs->name, ptData->Trader().c_str());
                    pArgs->p_len = password.size();
                    pArgs->password = (char*)calloc(1, pArgs->p_len + 1);
                    strcpy(pArgs->password, password.c_str());

                    struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
                    pResetStruct->create_new = true;

                    pResetStruct->new_incoming = 0;
                    pResetStruct->new_outgoing = 0;

                    pResetStruct->reset_hour = CUtility::GetLocalTimeHH();
                    pResetStruct->reset_min = CUtility::GetTimeMM() - 1;
                    pArgs->reset_time = pResetStruct;
                    m_LoginToPassword.Add(TraderData.Trader(), TraderData.Password());

                    CClientsManager::getInstance().AddClient(TraderData.Trader(), TraderData.Password(), false);

                    add_new_user(pAsyncServer,
                                 pArgs->name,
                                 pArgs->name_len,
                                 pArgs);

                    free (pArgs->name);
                    free (pArgs->password);
                    free (pArgs->reset_time);
                    free (pArgs);
                }*/
            }
            else
            {
                CThreadTraderData* ptTrader = m_TraderToTraderData.Update(TraderData.Trader(), ptData);

                tBalanceUpdate tbdata;

                tbdata.secType = ROMVALUE::SECTYPE_EQUITY;
                ptTrader->GetBalance(tbdata);
                ptData->SetBalance(tbdata);

                tbdata.secType = ROMVALUE::SECTYPE_OPTION;
                ptTrader->GetBalance(tbdata);
                ptData->SetBalance(tbdata);

                tbdata.secType = ROMVALUE::SECTYPE_FUTURE;
                ptTrader->GetBalance(tbdata);
                ptData->SetBalance(tbdata);

                tbdata.secType = ROMVALUE::SECTYPE_CURRENCY;
                ptTrader->GetBalance(tbdata);
                ptData->SetBalance(tbdata);

                ptData->ExemptTrader(ptTrader->Exempted());

                std::string password;
                ptTrader->GetPassword(password);

                if (password.compare(TraderData.Password()) != 0)
                {
                    CClientsManager::getInstance().UpdateClientPassword(ptTrader->Trader(), TraderData.Password());
                    m_LoginToPassword.Update(ptTrader->Trader(), TraderData.Password());

                    /*
                    struct init_args* pArgs = (struct init_args*)calloc(1, sizeof(struct init_args));
                    pArgs->name_len = TraderData.Trader().size();
                    pArgs->name = (char*)calloc(1, pArgs->name_len + 1);
                    strcpy(pArgs->name, TraderData.Trader().c_str());
                    pArgs->p_len = TraderData.Password().size();
                    pArgs->password = (char*)calloc(1, pArgs->p_len + 1);
                    strcpy(pArgs->password, TraderData.Password().c_str());

                    struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
                    pResetStruct->create_new = false;

                    pResetStruct->new_incoming = 0;
                    pResetStruct->new_outgoing = 0;

                    pResetStruct->reset_hour = CUtility::GetLocalTimeHH();
                    pResetStruct->reset_min = CUtility::GetTimeMM() - 1;
                    pArgs->reset_time = pResetStruct;

                    std::string dummy;
                    m_LoginToPassword.Remove(TraderData.Trader(), dummy);
                    m_LoginToPassword.Add(TraderData.Trader(), TraderData.Password());

                    alter_user(pAsyncServer,
                               pArgs->name,
                               pArgs->name_len,
                               pArgs);

                    free (pArgs->name);
                    free (pArgs->password);
                    free (pArgs->reset_time);
                    free (pArgs);
                    */
                }

                delete ptTrader;
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
    }

    return rc;
}

bool CClientProfileManager::LoadProfilesFromDB(bool reload)
{
    bool rc = true;
    queue_t results = create_queue();
    const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    char sqlStr[128];

    sprintf(sqlStr, "%s.dbo.ROMGetROMusersNew", db.database.c_str());

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    //if (CUtility::DbExec(sqlStr, db.dbserver.c_str(), db.user.c_str(), db.password.c_str(), &results))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        CRomMap<CThreadTraderData*> tempTraderToTraderData(false);

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            CTraderData TraderData;
            std::string strvalue;

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                if (pVal->type_in_union != NULL)
                {
                    switch(cnt)
                    {
                    case 0:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetTrader(strvalue);
                        }
                        break;
                    case 1:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetPassword(strvalue);
                        }
                        break;
                    case 4:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetReportingFirm(strvalue);
                        }
                        break;
                    case 5:
                        TraderData.SetReportingType((pVal->g_un).un_int);
                        break;
                    case 6:
                        TraderData.SetRoninMember((pVal->g_un).un_int);
                        break;
                    case 7:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.LoadSubTraders(strvalue);
                        }
                        break;
                    case 9:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetOtherTraders(strvalue);
                        }
                        break;
                    case 10:
                        if ((pVal->g_un).un_int == 0)
                        {
                            TraderData.SetDiscretionaryAbility(false);
                        }
                        else
                        {
                            TraderData.SetDiscretionaryAbility(true);
                        }
                        break;
                    case 11:
                        if ((pVal->g_un).un_int == 0)
                        {
                            TraderData.SetPrincipalAbility(false);
                        }
                        else
                        {
                            TraderData.SetPrincipalAbility(true);
                        }
                        break;
                    case 12:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            TraderData.SetGroup(strvalue);
                        }
                        break;
                    case 13:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 14:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 15:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 16:
                        TraderData.SetBuyingPower((pVal->g_un).un_double, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 17:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 18:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 19:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 20:
                        TraderData.SetMaxShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 21:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 22:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 23:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 24:
                        TraderData.SetMaxDollars((pVal->g_un).un_double, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 25:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 26:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 27:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 28:
                        TraderData.SetMaxOrders((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    case 29:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_EQUITY);
                        break;
                    case 30:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_OPTION);
                        break;
                    case 31:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_FUTURE);
                        break;
                    case 32:
                        TraderData.SetMaxTotalShares((pVal->g_un).un_int, ROMVALUE::SECTYPE_CURRENCY);
                        break;
                    default:
                        break;
                    }
                }

                cnt++;
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
            CThreadTraderData* ptData = new CThreadTraderData(&TraderData);
            tempTraderToTraderData.Add(TraderData.Trader(), ptData);

            if (true == reload)
            {
                CThreadTraderData* pData = NULL;

                if (m_TraderToTraderData.Get(ptData->Trader(), pData) == false)
                {
                    CClientsManager::getInstance().AddNewClient(TraderData.Trader(), TraderData.Password());
                    m_LoginToPassword.Add(TraderData.Trader(), TraderData.Password());
                }
                else
                {
                    std::string pwd;
                    pData->GetPassword(pwd);
                    if (TraderData.Password().compare(pwd) != 0)
                    {
                        CClientsManager::getInstance().UpdateClientPassword(TraderData.Trader(), TraderData.Password());
                        m_LoginToPassword.Update(TraderData.Trader(), TraderData.Password());
                    }
                }
            }
            else
            {
                m_LoginToPassword.Add(TraderData.Trader(), TraderData.Password());
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
                    tbdata.secType = ROMVALUE::SECTYPE_EQUITY;
                    ptTrader->GetBalance(tbdata);
                    ptData->SetBalance(tbdata);

                    tbdata.secType = ROMVALUE::SECTYPE_OPTION;
                    ptTrader->GetBalance(tbdata);
                    ptData->SetBalance(tbdata);

                    tbdata.secType = ROMVALUE::SECTYPE_FUTURE;
                    ptTrader->GetBalance(tbdata);
                    ptData->SetBalance(tbdata);

                    tbdata.secType = ROMVALUE::SECTYPE_CURRENCY;
                    ptTrader->GetBalance(tbdata);
                    ptData->SetBalance(tbdata);

                    std::string oldpwd, newpwd;
                    ptTrader->GetPassword(oldpwd);
                    ptData->GetPassword(newpwd);
                    ptData->SetBalance(tbdata);
                    ptData->ExemptTrader(ptTrader->Exempted());
                }
                it++;
                delete ptTrader;
            }
        }

        pthread_rwlock_unlock(&m_lock);
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
        rc = false;
    }

    destroy_queue(results);
    //m_ClearingActSecLimits.ReloadFromDB();

    return rc;
}

bool CClientProfileManager::AllowedToPlaceOrder(const CRomOrder& order, tBalanceUpdate& BUpdate, std::string& error)
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
            rc = !pData->ExceedLimit(order, BUpdate, error);
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
        rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrder&>(order).ClearingSecKey(), BUpdate, error));

        if (false == rc)
        {
            //back out the balance update from the per trader balance
            BUpdate.orderChange *= -1;
            BUpdate.shareChange *= -1;
            BUpdate.dollarChange *= -1;
            UpdateBalance(order.Trader(), BUpdate);
        }
    }

    return rc;
}

bool CClientProfileManager::AllowedToPlaceOrder_UserDefinedSpread(const CRomOrder& order, const vector<CCsv*>* pLegs, tBalanceUpdate& BUpdate, std::string& error)
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
            //if (order.SecType() != ROMVALUE::SECTYPE_COMPLEX)
            //{
                rc = !pData->ExceedLimit_Cplx(const_cast<CRomOrder&>(order), pLegs, BUpdate, error);
            //}
        }
    }
    else
    {
        error.append("Invalid Trader:");
        error.append(order.Trader());
        rc = false;
    }

    pthread_rwlock_unlock(&m_lock);

    //if (rc && CConfig::getInstance().ClearingLimitsEnabled() && order.SecType() != ROMVALUE::SECTYPE_COMPLEX)
    if (rc)
    {
        rc = !(m_ClearingActSecLimits.ExceedLimit(const_cast<CRomOrder&>(order).ClearingSecKey(), BUpdate, error));

        if (false == rc)
        {
            //back out the balance update from the per trader balance
            BUpdate.orderChange *= -1;
            BUpdate.shareChange *= -1;
            BUpdate.dollarChange *= -1;
            UpdateBalance(order.Trader(), BUpdate);
        }
    }

    return rc;
}


void CClientProfileManager::BackOutReplace(const CRomOrder& order)
{
    tBalanceUpdate theChange = {1, 0, 0, 'E'};
    theChange.secType = order.SecType();
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

    //if (CConfig::getInstance().ClearingLimitsEnabled())
    //{
        m_ClearingActSecLimits.UpdateBalance(const_cast<CRomOrder&>(order).ClearingSecKey(), theChange);
    //}
}


//bool CClientProfileManager::AllowedToReplaceOrder(const CRomOrder& order, tBalanceUpdate* pBUpdate, std::string& error)
bool CClientProfileManager::AllowedToReplaceOrder(tKeyedBalanceUpdate& update, std::string& error)
{
    //tBalanceUpdate theChange = {0, 0, 0, ROMVALUE::SECTYPE_EQUITY};
    //theChange.secType = order.SecType();
    //theChange.sideSign = (order.Side() == ROMVALUE::SIDE_BUY)? 1:-1;
    //theChange.shareChange = order.ShareChange() * order.Ratio();
    //theChange.orderChange = 0;

    //theChange.dollarChange = order.Multiplier() * order.CurrencyRate() * order.SideSign() * (order.PriceChange() * order.Leave() + order.ShareChange() * order.ReplacingPrice());

    CThreadTraderData* pData = NULL;
    bool rc = true;

    pthread_rwlock_rdlock(&m_lock);

    if (m_TraderToTraderData.Get(update.trader, pData))
    {
        rc = !pData->ExceedLimit(update.update, error);
    }

    pthread_rwlock_unlock(&m_lock);

    if (rc)
    {
        rc = !(m_ClearingActSecLimits.ExceedLimit(update.clearing_sec, update.update, error));

        if (false == rc)
        {
            //back out the balance update from the per trader balance
            update.update.orderChange *= -1;
            update.update.shareChange *= -1;
            update.update.dollarChange *= -1;
            UpdateBalance(update.trader, update.update);
        }
    }
    return rc;
}

bool CClientProfileManager::UpdateClearingActSecBalance(const std::string& key, const tBalanceUpdate& update)
{
    //if (CConfig::getInstance().ClearingLimitsEnabled() == 0)
    //{
    //    return true;
    //}
    bool rc = m_ClearingActSecLimits.UpdateBalance(key, update);
    return rc;
}

bool CClientProfileManager::GetClearingActSecLimits(const std::string& ClearingActSecKey, tLimits& limits)
{
    bool rc = m_ClearingActSecLimits.GetLimits(ClearingActSecKey, limits);
    return rc;
}

void* CClientProfileManager::ReloadClearingActSecLimits(void* pArg)
{
    std::string* pClearingActSec = (std::string*)pArg;
    CClientProfileManager::getInstance().ReloadClearingActSecLimits(pClearingActSec);

    if (pClearingActSec)
    {
        delete pClearingActSec;
    }

    return NULL;
}

void CClientProfileManager::ReloadClearingActSecLimits(const std::string* key)
{
    m_ClearingActSecLimits.ReloadFromDB(key);
}

CThreadLimits::CThreadLimits(const std::string& ClearingSecKey, const tLimits& limits)
{
    m_limits.Balance = limits.Balance;
    m_limits.BuyingPower = limits.BuyingPower;
    m_limits.MaxDollars = limits.MaxDollars;
    m_limits.MaxOrders = limits.MaxOrders;
    m_limits.MaxShares = limits.MaxShares;
    m_limits.MaxTotalShares = limits.MaxTotalShares;
    m_limits.NumOrders = limits.NumOrders;
    m_limits.NumShares = limits.NumShares;
    m_ClearingSecKey = ClearingSecKey;

    pthread_rwlock_init(&m_lock, NULL);
}

CThreadLimits::~CThreadLimits()
{
    pthread_rwlock_destroy(&m_lock);
}

bool CThreadLimits::ExceedLimit(const tBalanceUpdate& BUpdate, std::string& error)
{
    pthread_rwlock_wrlock(&m_lock);

    double dollarChange = BUpdate.dollarChange;
    char errMsg[128];

    int newNumOrders = m_limits.NumOrders + BUpdate.orderChange;
    long newNumShares = m_limits.NumShares + BUpdate.shareChange;

    //MaxOrder
    if (m_limits.MaxOrders < newNumOrders)
    {
        sprintf(errMsg, "Exceeds %s's MaxOrders:%d", m_ClearingSecKey.c_str(), m_limits.MaxOrders);
        error = errMsg;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }

    //MaxShares
    if (BUpdate.shareChange > m_limits.MaxShares)
    {
        sprintf(errMsg, "Order's Shares Exceeds %s's Max Shares:%ld", m_ClearingSecKey.c_str(), m_limits.MaxShares);
        error = errMsg;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }

    //MaxDollars
    if (dollarChange > m_limits.MaxDollars)
    {
        sprintf(errMsg, "Order Exceeds %s's Max Dollars Allowed Per Order:%f", m_ClearingSecKey.c_str(), m_limits.MaxDollars);
        error = errMsg;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }

    //Balance
    double newBalance = m_limits.Balance + dollarChange;

    if (newBalance > m_limits.BuyingPower ||
            newBalance < -1 * m_limits.BuyingPower)
    {
        sprintf(errMsg, "Exceeds %s's Buying Power:%f", m_ClearingSecKey.c_str(), m_limits.BuyingPower);
        error = errMsg;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }

    //MaxTotalShares
    if (newNumShares > m_limits.MaxTotalShares)
    {
        sprintf(errMsg, "Exceeds %s's Max Total Shares:%ld", m_ClearingSecKey.c_str(), m_limits.MaxTotalShares);
        error = errMsg;
        pthread_rwlock_unlock(&m_lock);
        return true;
    }

    m_limits.Balance = newBalance;
    m_limits.NumOrders = newNumOrders;
    m_limits.NumShares = newNumShares;

    pthread_rwlock_unlock(&m_lock);

    return false;
}

void CThreadLimits::UpdateBalance(const tBalanceUpdate& BUpdate)
{
    pthread_rwlock_wrlock(&m_lock);

    m_limits.Balance += BUpdate.dollarChange;
    m_limits.NumOrders += BUpdate.orderChange;
    m_limits.NumShares += BUpdate.shareChange;

    pthread_rwlock_unlock(&m_lock);
}

void CThreadLimits::GetLimits(tLimits& limits)
{
    pthread_rwlock_rdlock(&m_lock);

    limits.Balance = m_limits.Balance;
    limits.BuyingPower = m_limits.BuyingPower;
    limits.MaxDollars = m_limits.MaxDollars;
    limits.MaxOrders = m_limits.MaxOrders;
    limits.MaxShares = m_limits.MaxShares;
    limits.MaxTotalShares = m_limits.MaxTotalShares;
    limits.NumOrders = m_limits.NumOrders;
    limits.NumShares = m_limits.NumShares;

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

 bool CThreadLimits::Exempt(const std::string& key, bool action)
 {
     bool rc = false;
     
     return rc;
 }
 
 bool CThreadLimits::Exempted(const std::string& key)
 {
     bool rc = false;
     
     return rc;
 }
CClearingActLimits::CClearingActLimits() : m_ClearingSecToLimits(false)
{
    pthread_rwlock_init(&m_lock, NULL);
}

CClearingActLimits::~CClearingActLimits()
{
    pthread_rwlock_destroy(&m_lock);
}

bool CClearingActLimits::ExceedLimit(const std::string& key, const tBalanceUpdate& BUpdate, std::string& error)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    bool rc = false;

    if (m_ClearingSecToLimits.Get(key, pLimits))
    {
        rc = pLimits->ExceedLimit(BUpdate, error);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClearingActLimits::UpdateBalance(const std::string& key, const tBalanceUpdate& BUpdate)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    if (m_ClearingSecToLimits.Get(key, pLimits))
    {
        pLimits->UpdateBalance(BUpdate);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClearingActLimits::GetLimits(const std::string& ClearingSecKey, tLimits& limits)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;
    if (m_ClearingSecToLimits.Get(ClearingSecKey, pLimits))
    {
        rc = true;
        pLimits->GetLimits(limits);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CClearingActLimits::AddLimits(const std::string& ClearingSecKey, const tLimits& limits)
{
    CThreadLimits* pLimits = new CThreadLimits(ClearingSecKey, limits);

    pthread_rwlock_wrlock(&m_lock);

    bool rc = false;

    if (m_ClearingSecToLimits.Find(ClearingSecKey) == false)
    {
        rc = m_ClearingSecToLimits.Add(ClearingSecKey, pLimits);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CClearingActLimits::ReloadFromDB(const std::string* key)
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


void CClearingActLimits::ReloadAllLimits()
{
    queue_t results = create_queue();
    const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    char sqlStr[128];

    sprintf(sqlStr, "select * from %s.dbo.ClearingActToLimits where active=1", db.database.c_str());
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    //if (CUtility::DbExec(sqlStr, db.dbserver.c_str(), db.user.c_str(), db.password.c_str(), &results))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, 0};
        std::string key;
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        CRomMap<CThreadLimits*> oldMap;
        m_ClearingSecToLimits.Swap(oldMap);

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            CTraderData TraderData;

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
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            AddLimits(key, limits);

            free(rowE);
            destroy_queue(row);
        }

        std::list<CThreadLimits*> oldData;
        oldMap.GetAll(oldData);

        std::list<CThreadLimits*>::iterator it = oldData.begin();
        CThreadLimits* ptLimits = NULL;
        CThreadLimits* ptlimits = NULL;

        while (it != oldData.end())
        {
            ptLimits = *it;

            if (m_ClearingSecToLimits.Get(ptLimits->ClearingSeckey(), ptlimits))
            {
                tLimits Limits = {0, 0, 0, 0, 0, 0, 0, 0};
                ptLimits->GetLimits(Limits);
                tBalanceUpdate update;
                update.dollarChange = Limits.Balance;
                update.orderChange = Limits.NumOrders;
                update.shareChange = Limits.NumShares;
                ptlimits->UpdateBalance(update);
            }

            delete ptLimits;
            it++;
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
}

void CClearingActLimits::ReloadLimits(const std::string* key)
{
    queue_t results = create_queue();
    const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    char sqlStr[128];
    int index = key->find('/');

    std::string clearing = key->substr(0, index);
    std::string type = key->substr(index + 1);

    sprintf(sqlStr, "select * from %s.dbo.ClearingActToLimits where active=1 and ClearingAcID='%s' and Type='%s'",
            db.database.c_str(), clearing.c_str(), type.c_str());

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    //if (CUtility::DbExec(sqlStr, db.dbserver.c_str(), db.user.c_str(), db.password.c_str(), &results))
    {
        tLimits limits = {0, 0, 0, 0, 0, 0, 0, 0};
        std::string Key;
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;

        if (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            CTraderData TraderData;

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
                            Key.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 1:
                        if (pVal->arr_len)
                        {
                            Key.append("/");
                            Key.append((pVal->g_un).un_array, pVal->arr_len);;
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
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            if (UpdateLimits(Key, limits) == false)
            {
                AddLimits(Key, limits);
            }

            free(rowE);
            destroy_queue(row);
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
}

bool CClearingActLimits::UpdateLimits(const std::string& ClearingSecKey, const tLimits& limits)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    CThreadLimits* pLimits = NULL;

    if (m_ClearingSecToLimits.Get(ClearingSecKey, pLimits))
    {
        pLimits->UpdateLimits(limits);
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}
