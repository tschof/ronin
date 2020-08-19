#include "ThreadRomOrdersNew.h"

#include "RomCancel.h"
#include "RomReject.h"
#include "ClientProfileManagerNew.h"
#include "ClientsManager.h"
#include "DestsManager.h"
#include "romfielddefs.h"
//#include "GStore.h"
//#include "generic_db.h"
#include "debug_logger.h"
#include "RomMapN.h"
#include "BuffStore.h"
#include "fix_rom_trans.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h>
#include "MDSClient.h"
#include "PositionManager.h"


typedef struct 
{
    CRomOrderNew* body;
    std::vector<CRomOrderNew*> legs;
} tUserDefinedSpread;

CThreadRomOrdersNew::~CThreadRomOrdersNew()
{
    CleanUp();

    std::set<void*>::iterator it = m_OrderSets.begin();
    void* pOrders = NULL;

    while (it != m_OrderSets.end())
    {
        pOrders = *it;
        free (pOrders);
        it++;
    }

    pthread_rwlock_destroy(&m_lock);
}

void CThreadRomOrdersNew::ClearMaps()
{
    std::list<CThreadOrderNew*> orders;
    m_RomTagToOrder.GetAll(orders);
    std::list<CThreadOrderNew*>::iterator it = orders.begin();
    while (it != orders.end())
    {
        delete *it;
        it++;
    }

    if (m_FreeList.IsEmpty() == false)
    {
        std::deque<CThreadOrderNew*> freeOrders;
        m_FreeList.GetAll(freeOrders);
        std::deque<CThreadOrderNew*>::iterator it = freeOrders.begin();
        while (it != freeOrders.end())
        {
            delete *it;
            it++;
        }
    }

    m_RomTagToOrder.RemoveAll();
    m_ClientTagToOrder.RemoveAll();

    tOrderSet* pOrders = NULL;

    while ((pOrders = (tOrderSet*)m_DestToOrderSet.RemoveNext()) != NULL)
    {
        delete pOrders;
    }

    while ((pOrders = (tOrderSet*)m_OwnerToOrderSet.RemoveNext()) != NULL)
    {
        delete pOrders;
    }
}

void CThreadRomOrdersNew::MapOrder(CThreadOrderNew* ptOrder, CRomOrderNew* pOrder)
{
    m_RomTagToOrder.Add(pOrder->RomTag(), ptOrder);

    if (strlen(pOrder->ParentRomTag()) == 0)
    {
        std::string theTag;
        MakeTheClientTag(theTag, pOrder->Owner(), pOrder->ClientTag());

        m_ClientTagToOrder.Add(theTag, ptOrder);

        tOrderSet* pOrderSet = NULL;

        if (m_OwnerToOrderSet.Get(pOrder->Owner(), pOrderSet))
        {
            pOrderSet->Add(ptOrder);
        }
        else
        {
            pOrderSet = new tOrderSet;
            m_OwnerToOrderSet.Add(pOrder->Owner(), pOrderSet);
            pOrderSet->Add(ptOrder);
        }

        if (m_DestToOrderSet.Get(pOrder->Dest(), pOrderSet))
        {
            pOrderSet->Add(ptOrder);
        }
        else
        {
            pOrderSet = new tOrderSet;
            m_DestToOrderSet.Add(pOrder->Dest(), pOrderSet);
            pOrderSet->Add(ptOrder);
        }
    }
}

CThreadOrderNew* CThreadRomOrdersNew::StoreOrder(CRomOrderNew* pOrder)
{
    pthread_rwlock_wrlock(&m_lock);

    if (m_FreeList.IsEmpty())
    {
        ExpandFreeList();
    }

    CThreadOrderNew* ptOrder = NULL;
    m_FreeList.Get(ptOrder);
    ptOrder->SetOrder(pOrder);
    MapOrder(ptOrder, pOrder);

    pthread_rwlock_unlock(&m_lock);

    return ptOrder;
}

CThreadRomOrdersNew::CThreadRomOrdersNew():
    m_RomTagToOrder(true),
    m_ClientTagToOrder(true),
    m_OwnerToOrderSet(true),
    m_DestToOrderSet(true),
    m_ProductMultiplier(true),
    m_OrderNum(1, true)
{
    CRomOrderNew::Init();
    pthread_rwlock_init(&m_lock, NULL);
    SetDateInstanceStr();

    InitOrderNum();

    ExpandFreeList();

    LoadProductMultiplierMap();
        
    //if (CConfig::getInstance().QueryLiveOrderFromOats() == 0)
    //{
    //    LoadOrdersFromDB();
    //}
}

/*void CThreadRomOrdersNew::DestroyOrder(CRomOrderNew* pOrder)
{
    pthread_rwlock_wrlock(&m_lock);

    destroyorder(pOrder);

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrdersNew::destroyorder(CRomOrderNew* pOrder)
{
    CThreadOrder* ptOrder = NULL;
    m_RomTagToOrder.Remove(pOrder->RomTag(), ptOrder);

    if (ptOrder != NULL &&
        strlen(pOrder->ParentRomTag()) == 0)
    {
        CThreadSet<CThreadOrder*>* pOrders = NULL;
        char value[200];
        sprintf(value, "%s%s", pOrder->Owner(), pOrder->ClientTag());
        m_ClientTagToOrder.Remove(value, ptOrder);

        if (m_OwnerToOrderSet.Get(pOrder->Owner(), pOrders))
        {
            pOrders->Remove(ptOrder);
        }

        if (m_DestToOrderSet.Get(pOrder->Dest(), pOrders))
        {
            pOrders->Remove(ptOrder);
        }

        m_FreeList.Add(ptOrder);
    }
}*/

CRomOrderNew* CThreadRomOrdersNew::RemoveOrder(const std::string& romTag)
{
    pthread_rwlock_wrlock(&m_lock);

    CRomOrderNew* pOrder = removeOrder(romTag);

    pthread_rwlock_unlock(&m_lock);

    return pOrder;
}

CRomOrderNew* CThreadRomOrdersNew::removeOrder(const std::string& romTag)
{
    CThreadOrderNew* ptOrder = NULL;
    m_RomTagToOrder.Remove(romTag, ptOrder);
    CRomOrderNew* rc = NULL;

    //if (ptOrder != NULL &&
    //    strlen(pOrder->ParentRomTag()) == 0)
    if (ptOrder != NULL)
    {
        short cplxType = ptOrder->GetCplxType();
        if (cplxType != ROMVALUE::CPLXTYPE_LEG) //legs are only in romTag map!!!
        {
            CThreadSet<CThreadOrderNew*>* pOrders = NULL;
            char owner[128], dest[10], clientTag[128];
            ptOrder->GetOwner(owner, sizeof(owner));
            ptOrder->GetDest(dest, sizeof(dest));
            ptOrder->GetClientTag(clientTag, sizeof(clientTag));
            
            std::string theTag;
            MakeTheClientTag(theTag, owner, clientTag);

            m_ClientTagToOrder.Remove(theTag, ptOrder);

            if (NULL != ptOrder)
            {
                if (m_OwnerToOrderSet.Get(owner, pOrders))
                {
                    pOrders->Remove(ptOrder);
                }

                if (m_DestToOrderSet.Get(dest, pOrders))
                {
                    pOrders->Remove(ptOrder);
                }
            }
        }

        rc = ptOrder->Reset();
        m_FreeList.Add(ptOrder);
    }

    return rc;
}



/*
bool CThreadRomOrdersNew::AllocateMemoryMap()
{
    bool rc = false;
    unsigned long step = CConfig::getInstance().OrdersAllocStep();
    void* pOrders = calloc(step, sizeof(CRomOrderNew));

    if (pOrders)
    {
        m_OrderSets.insert(pOrders);
        CRomOrderNew* pOrder = (CRomOrderNew*)pOrders;

        for (unsigned int i = 0; i < step; i++)
        {
            pOrder[i].SetUsed(false);
            CThreadOrder* ptOrder = new CThreadOrder(&(pOrder[i]));
            m_FreeList.Add(ptOrder);
        }
        rc = true;
    }

    return rc;
}
*/

void CThreadRomOrdersNew::ExpandFreeList()
{
    int step = CConfig::getInstance().OrdersAllocStep();
    CThreadOrderNew* ptOrder = NULL;

    for (int i = 0; i < step; i++)
    {
        ptOrder = new CThreadOrderNew();
        m_FreeList.Add(ptOrder);
    }
}

void CThreadRomOrdersNew::BuildBalanceUpdateFromOrder(const CRomOrderNew& order, tBalanceUpdate& update)
{
    if (CRomOrderNew::IsLimitType(order.Type()))
    {
        update.dollarChange = order.SideSign() * order.Multiplier() * order.CurrencyRate() * order.Leave() * order.Price();
    }
    else
    {
        update.dollarChange = 0;
    }
    update.secIndex = order.SecIndex();
    update.orderChange = 1;
    update.shareChange = order.Leave();
}

bool CThreadRomOrdersNew::GetOrderFromRomTag(const char* tag, CRomOrderNew* pOrder)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        ptOrder->GetOrder(pOrder);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrdersNew::GetOrderFromClientTag(const std::string& tag, CRomOrderNew* pOrder)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_ClientTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        ptOrder->GetOrder(pOrder);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

/*
bool CThreadRomOrdersNew::GetNextRomExecTag(const std::string& romTag, std::string& romExecTag)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;

    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->GetNextRomExecTag(romExecTag);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}
*/
void CThreadRomOrdersNew::BuildCancels(CCsv& filter, std::set<std::string>& tags, bool dayorderonly)
{
    pthread_rwlock_rdlock(&m_lock);

    std::string dest = "-1";
    filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
    std::string owner;
    tOrderSet* ptOrderSet = NULL;

    int ownerCancel = 0;
    filter.GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownerCancel);

    if (filter.GetAt(ROMFIELDS::CSV_OWNER, owner) == true)
    {
        m_OwnerToOrderSet.Get(owner, ptOrderSet);
    }

    if (NULL == ptOrderSet &&
            dest != "-1")
    {
        m_DestToOrderSet.Get(dest, ptOrderSet);
    }

    if (NULL == ptOrderSet)
    {
        pthread_rwlock_unlock(&m_lock);
        return;
    }

    std::set<CThreadOrderNew*> orderSet;
    ptOrderSet->GetAll(orderSet);
    std::set<CThreadOrderNew*>::iterator it = orderSet.begin();
    CThreadOrderNew* ptOrder = NULL;
    CRomOrderNew order;
    std::string err = "";

    while (it != orderSet.end())
    {
        ptOrder = *it;
        //ptOrder->GetOrder(&order);

        if (ptOrder->DeadSpread() ||
            (dayorderonly &&
             (ptOrder->TIF() == ROMVALUE::TIF_GTC || ptOrder->TIF() == ROMVALUE::TIF_GTD)))
        {
            it++;
            continue;
        }

        if (ptOrder->Filter(filter) == false)
        {
            /*
            CCsv* pCancel = CCsvStore::getInstance().Get();
            pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
            pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, order.RomTag());
            pCancel->SetAt(ROMFIELDS::CSV_TAG, order.ClientTag());
            pCancel->SetAt(ROMFIELDS::CSV_OWNER, order.Owner());


            if (strcmp(order.Owner(), owner.c_str()) == 0)
            {
                short ownercancel = 0;
                filter.GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
                pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
            }

            pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            */
            std::string tag;
            if (ownerCancel)
            {
                ptOrder->GetClientTag(tag);
            }
            else
            {
                ptOrder->GetRomTag(tag);
            }
            tags.insert(tag);
        }
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}

//void CThreadRomOrdersNew::BuildDestCancels(const std::string& destID, std::deque<COatsMsg*>& oatsMsgs, std::set<CCsv*>& cancels)
void CThreadRomOrdersNew::BuildDestCancels(const std::string& destID, std::set<std::string>& romTags)
{
    pthread_rwlock_rdlock(&m_lock);

    tOrderSet* ptOrderSet = NULL;
    if (m_DestToOrderSet.Get(destID, ptOrderSet))
    {
        std::set<CThreadOrderNew*> orderSet;
        ptOrderSet->GetAll(orderSet);
        std::set<CThreadOrderNew*>::iterator it = orderSet.begin();
        CThreadOrderNew* ptOrder = NULL;
        CRomOrderNew order;

        char omTime[32];
        CUtility::GetGmtTimeStr(omTime, sizeof(omTime));

        while (it != orderSet.end())
        {
            ptOrder = *it;
            //ptOrder->GetOrder(&order);

            if (ptOrder->TIF() != ROMVALUE::TIF_GTD &&
                ptOrder->TIF() != ROMVALUE::TIF_GTC &&
                ptOrder->DeadSpread() == false)
            {
                /*
                CCsv* pCancel = CCsvStore::getInstance().Get();
                pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
                pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, order.RomTag());
                pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 0);
                //order.GetCsv(*pCancel);

                pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
                */
                std::string romTag;
                ptOrder->GetRomTag(romTag);
                romTags.insert(romTag);
            }

            it++;
        }
    }
    pthread_rwlock_unlock(&m_lock);
}

//void CThreadRomOrdersNew::BuildAllCancels(std::deque<COatsMsg*>& oatsMsgs, std::set<CRomCancel*>& cancels)
//{
//}

/*
void CThreadRomOrdersNew::Cancels(CCsv& filter, std::list<COatsMsg*>& oatsMsgs)
{
  pthread_rwlock_rdlock(&m_lock);

  std::string dest = "-1";
  filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
  std::string owner;
  tOrderSet* ptOrderSet = NULL;

	if (filter.GetAt(ROMFIELDS::CSV_OWNER, owner) == true)
	{
		m_OwnerToOrderSet.Get(owner, ptOrderSet);
	}

	if (NULL == ptOrderSet &&
		  dest != "-1")
	{
		m_DestToOrderSet.Get(dest, ptOrderSet);
	}

	if (NULL == ptOrderSet)
	{
    pthread_rwlock_unlock(&m_lock);
		return;
	}

  std::set<CThreadOrder*> orderSet;
  ptOrderSet->GetAll(orderSet);
  std::set<CThreadOrder*>::iterator it = orderSet.begin();
	CThreadOrder* ptOrder = NULL;
  CRomOrderNew order;
	std::string err = "";

	while (it != orderSet.end())
	{
		ptOrder = *it;
    ptOrder->GetOrder(&order);

		if (order.Status() == ROMVALUE::STATUS_FILLED)
		{
			it++;
			continue;
		}

		if (Filter(order, filter) == false)
		{
			CRomCancel cancel(order);
      if (strcmp(order.Owner(), owner.c_str()) == 0)
      {
        short ownercancel = 0;
        filter.GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
			  cancel.SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
      }

      //CDestsManager::getInstance().Send(order.Dest(), cancel, err);

			cancel.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
			cancel.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
      std::string newtag;
      cancel.GetAt(ROMFIELDS::CSV_NEW_TAG, newtag);
			order.SetActClientTag(newtag.c_str());
			std::string omExTag;
      GetNextRomExecTag(order.RomTag(), omExTag);
			cancel.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
      COatsMsg* pMsg = new COatsMsg(&cancel, &order, true);
      oatsMsgs.push_back(pMsg);

      cancel.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
			cancel.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
      CDestsManager::getInstance().Send(order.Dest(), cancel, err);
		}
		it++;
	}

  pthread_rwlock_unlock(&m_lock);
}
*/

bool CThreadRomOrdersNew::Filter(const CRomOrderNew& order, CCsv& filter)
{
    char secType;

    if (filter.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType) == true )//filter.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE) == false)
    {
        if ((CUtility::IsEquityType(secType) == true &&
                CUtility::IsEquityType(order.SecType()) != true) ||
                (CUtility::IsOptionType(secType) == true &&
                 CUtility::IsOptionType(order.SecType()) != true) ||
                (CUtility::IsFutureType(secType) == true &&
                 CUtility::IsFutureType(order.SecType()) != true) ||
                (CUtility::IsCurrencyType(secType) == true &&
                 CUtility::IsCurrencyType(order.SecType()) != true)) // ||
                //(secType == ROMVALUE::SECTYPE_COMPLEX &&
                //order.CplxType() == ROMVALUE::CPLXTYPE_SINGLE))
        {
            return true;
        }
    }

    std::string exchange;
    if (filter.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_EXCHANGE) == false &&
            strcmp(order.Exchange(), exchange.c_str()) != 0)
    {
        return true;
    }

    std::string dest;
    if (filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST) == false &&
            strcmp(order.Dest(), dest.c_str()) != 0)
    {
        return true;
    }

    std::string trader;
    if (filter.GetAt(ROMFIELDS::CSV_TRADER, trader) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TRADER) == false &&
            strcmp(order.Trader(), trader.c_str()) != 0)
    {
        return true;
    }

    std::string tradefor;
    if (filter.GetAt(ROMFIELDS::CSV_TRADE_FOR, tradefor) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TRADE_FOR) == false &&
            strcmp(order.TradeFor(), tradefor.c_str()) != 0)
    {
        return true;
    }

    std::string clearingacct;
    if (filter.GetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, clearingacct) == true && // filter.IsFieldEmpty(ROMFIELDS::CSV_CLEARINGACCOUNT) == false &&
            strcmp(order.ClearingAccount(), clearingacct.c_str()) != 0)
    {
        return true;
    }

    std::string symbol;
    if (filter.GetAt(ROMFIELDS::CSV_SYMBOL, symbol) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_SYMBOL) == false &&
            strcmp(order.Symbol(), symbol.c_str()) != 0)
    {
        return true;
    }

    short side = 0;
    if (filter.GetAt(ROMFIELDS::CSV_SIDE, side) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_SIDE) == false &&
            order.Side() != side)
    {
        return true;
    }

    short type = 0;
    if (filter.GetAt(ROMFIELDS::CSV_TYPE, type) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TYPE) == false &&
            order.Type() != type)
    {
        return true;
    }

    double price = 0;
    if (filter.GetAt(ROMFIELDS::CSV_PRICE, price) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false &&
            order.Price() != price)
    {
        return true;
    }

    long origshare = 0;
    if (filter.GetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, origshare) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_ORIGINAL_SHARES) == false &&
            order.OriginalShares() != origshare)
    {
        return true;
    }

    short tif;
    if (filter.GetAt(ROMFIELDS::CSV_TIF, tif) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TIF) == false &&
            order.TIF() != tif)
    {
        return true;
    }

    if (CUtility::IsOptionType(order.SecType()))
    {
        std::string underlying;
        if (filter.GetAt(ROMFIELDS::CSV_UNDERLYING, underlying) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING) == false &&
                strcmp(order.Underlying(), underlying.c_str()) != 0)
        {
            return true;
        }

        /*double strike = 0, lstrike = 0;
        if (filter.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike) == true)
        {
            if (order.GetStrikePrice(lstrike))
            {
                if (lstrike != strike)
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }*/
        std::string strikeprice;
        if (filter.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strikeprice) == true)
        {
            if (strcmp(order.StrikePrice(), strikeprice.c_str()))
            {
                return true;
            }
        }

        char callput;
        if (filter.GetAt(ROMFIELDS::CSV_CALLPUT, callput) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_CALLPUT) == false &&
                order.CallOrPut() != callput)
        {
            return true;
        }
    }

    if (CUtility::IsEquityType(order.SecType()) == false)
    {
        std::string expmonth;
        if (filter.GetAt(ROMFIELDS::CSV_EXPYEARMONTH, expmonth) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_EXPYEARMONTH) == false &&
                strcmp(order.ExpYearMonth(), expmonth.c_str()) != 0) //filter.Get(ROMFIELDS::CSV_EXPYEARMONTH)) != 0)
        {
            return true;
        }
    }

    return false;
}

void CThreadRomOrdersNew::CleanUp()
{
    ClearMaps();
}

void CThreadRomOrdersNew::InitOrderNum()
{
    if (CConfig::getInstance().IsFirstRun() == false)
    {
        time_t ticks = time(NULL);
        struct tm *tmStruct = gmtime(&ticks);
        unsigned long seconds = 3600 * tmStruct->tm_hour + 60 * tmStruct->tm_min + tmStruct->tm_sec + 1;
        m_OrderNum.Reset(1000000 * seconds);
    }
}

bool CThreadRomOrdersNew::SetDateInstanceStr()
{
    bool rc = false;
    time_t t;
    time(&t);
    struct tm tmNow, tmCentralTime;
    localtime_r(&t, &tmNow);
    CUtility::AdjustToCentralTime(tmNow, tmCentralTime);

    std::stringstream stm;
    stm << CConfig::getInstance().InstanceName().c_str()
        << std::setw(3) << std::setfill('0') << 1900 + tmCentralTime.tm_year - 2000
        << std::setw(2) << std::setfill('0') << tmCentralTime.tm_mon+1
        << std::setw(2) << std::setfill('0') << tmCentralTime.tm_mday ;

    if (m_DateInstanceStr.compare(stm.str()) != 0)
    {
        m_DateInstanceStr = stm.str();
        rc = true;
    }

    return rc;
}

void CThreadRomOrdersNew::GetNextRomTag(const std::string& owner, std::string& romTag)
{
    pthread_rwlock_wrlock(&m_lock);

    if (CUtility::DayTurn_Central())
    {
        if (SetDateInstanceStr())
        {
            m_OrderNum.Reset(1);
        }
    }

    pthread_rwlock_unlock(&m_lock);

    unsigned long num = m_OrderNum.Increment();
    char value[200];
    sprintf(value, "%s-%s-%lu", owner.c_str(), CConfig::getInstance().SessionDateInstanceStr().c_str(),  num); //%lu???

    romTag = value;
}

void CThreadRomOrdersNew::ResetReplacingFields(const std::string& romTag)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;

    if (m_RomTagToOrder.Get(romTag, ptOrder))
    {
        ptOrder->ResetReplacingFields();
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrdersNew::UpdateClientTag(const std::string& oldtag, const std::string& newtag)
{
    if (oldtag.empty() == true ||
        newtag.compare(oldtag) == 0)
    {
        return;
    }

    pthread_rwlock_wrlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;

    if (m_ClientTagToOrder.Remove(oldtag, ptOrder))
    {
        m_ClientTagToOrder.Add(newtag, ptOrder);
    }

    pthread_rwlock_unlock(&m_lock);
}

//bool CThreadRomOrdersNew::UpdateOrder(const std::string& romTag, CCsv& msg, CRomOrderNew* pOrder, std::string& err)

/*bool CThreadRomOrdersNew::UpdateOrder(const std::string& romTag, CCsv& msg, std::string& dropStr, tKeyedBalanceUpdate& balance, std::string& err)
{
    pthread_rwlock_rdlock(&m_lock);

    bool rc = true;
    CThreadOrder* ptOrder = NULL;

    if (m_RomTagToOrder.Get(romTag, ptOrder))
    {
        char cmd = 'S';
        if (msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd) &&
            cmd == ROMVALUE::COMMAND_ZAP)
        {
            if (ptOrder->GetStatus() == ROMVALUE::STATUS_NEW)
            {
                msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
            }
            else
            {
                msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
            }
            msg.SetAt(ROMFIELDS::CSV_TEXT, "Zapped");
            msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        }

        short cplxType = ptOrder->GetCplxType();

        short cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
        msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

        if (!(cplxType == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD &&
                cplxtype == ROMVALUE::CPLXTYPE_LEG))
        {
            //the UpdateOrder should take care of sending oats messages
            //ptOrder->UpdateOrder(msg, pOrder);
            ptOrder->UpdateOrder(msg, dropStr, balance);

            if (cplxtype == ROMVALUE::CPLXTYPE_LEG &&
                msg.IsFieldEmpty(ROMFIELDS::CSV_PARENT_STAFFORD_TAG) == false)
            {
                if ((ptOrder->GetStatus() == ROMVALUE::STATUS_FILLED ||
                    ptOrder->GetStatus() == ROMVALUE::STATUS_PARTIALLYFILLED) &&
                    ptOrder->UpdateParent())
                {
                    long lastShares = 0;
                    msg.GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
                    long parentLastShares = lastShares / ptOrder->Ratio();
                    //fill parent: send body fill to client and oats
                }
            }
        }
        else
        {
            //should do ptOrder->SendOatsMsg
            ptOrder->SendOatsMsg(msg);
            //std::string romExTag;
            //ptOrder->GetOrderWithRomExTag(pOrder, romExTag);
            //msg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
        }
    }
    else
    {
        err = "Order not found";
        rc = false;
    }

    pthread_rwlock_unlock(&m_lock);

    if (rc == false)
    {
        return rc;
    }

    if (ptOrder->Dead())
    {
        CRomOrderNew* pRomOrder = RemoveOrder(romTag);
        if (pRomOrder)
        {
            CGStore::getInstance().Return(pRomOrder);
        }
    }

    return rc;
}*/
void CThreadRomOrdersNew::SetOrder(CRomOrderNew* pOrder, queue_t row)
{
    gendb_v* pVal = NULL;
    element_t colE;
    std::string strvalue;
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
                pOrder->SetAvgPrice((pVal->g_un).un_double);
                break;
            case 1:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetCallOrPut(strvalue[0]);
                }
                break;
            case 2:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetCapacity(strvalue[0]);
                }
                break;
            case 3:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetClearingAccount(strvalue.c_str());
                }
                break;
            case 4:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetClearingId(strvalue.c_str());
                }
                break;
            case 5:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetTimeStamp(strvalue.c_str());
                }
                break;
            case 6:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetCMTAAccount(strvalue.c_str());
                }
                break;
            case 7:
                pOrder->SetCum((pVal->g_un).un_int);
                break;
            case 8:
                {
                    char destStr[4];
                    sprintf(destStr, "%d", (pVal->g_un).un_int);
                    pOrder->SetExchange(destStr);
                }
                break;
            case 9:
                {
                    char destStr[4];
                    sprintf(destStr, "%d", (pVal->g_un).un_int);
                    pOrder->SetDest(destStr);
                }
                break;
            case 10:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetExDest(strvalue.c_str());
                }
                break;
            case 11:
                if (pVal->arr_len)
                {
                    //strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    //pOrder->SetExpYearMonth(strvalue.c_str());

                    //strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    //char yymm[7];
                    //sprintf(yymm, "%s%s", 
                    //        strvalue.substr(0, 4).c_str(),
                    //        strvalue.substr(5, 2).c_str());
                    //pOrder->SetExpYearMonth(yymm);
                }
                break;
            case 12:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetFirm(strvalue.c_str());
                }
                break;
            case 13:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetGiveUpAccount(strvalue.c_str());
                }
                break;
            case 14:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetInstructions(strvalue.c_str());
                }
                break;
            case 15:
                pOrder->SetLeave((pVal->g_un).un_int);
                pOrder->SetShares(pOrder->Cum() + pOrder->Leave());
                break;
            case 16:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetLocalAccount(strvalue.c_str());
                }
                break;
            case 17:
                {
                    char maxFloor[10];
                    sprintf(maxFloor, "%d", (pVal->g_un).un_int);
                    pOrder->SetMaxFloor(maxFloor);
                }
                break;
            case 18:
                pOrder->SetMinQty((pVal->g_un).un_int);
                break;
            case 19:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetMMAccount(strvalue.c_str());
                }
                break;
            case 20:
                pOrder->SetMMExchange((pVal->g_un).un_int);
                break;
            case 21:
                pOrder->SetMultiplier((pVal->g_un).un_double);
                break;
            case 22:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetRomTag(strvalue.c_str());
                }
                break;
            case 23:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetOpenClose(strvalue.c_str());
                }
                break;
            case 24:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    //need to reformat it!!
                    char exptime[32];
                    sprintf(exptime, "%s%s%s-%s:%s:%s",
                            strvalue.substr(0, 4).c_str(),
                            strvalue.substr(5, 2).c_str(),
                            strvalue.substr(8, 2).c_str(),
                            strvalue.substr(11, 2).c_str(),
                            strvalue.substr(14, 2).c_str(),
                            strvalue.substr(17, 2).c_str());
                    pOrder->SetExpTime(exptime);
                }
                break;
            case 25:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetRomExchangeTag(strvalue.c_str());
                }
                break;
            case 26:
                pOrder->SetType((pVal->g_un).un_int);
                break;
            case 27:
                pOrder->SetOriginalShares((pVal->g_un).un_int);
                break;
            case 28:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetOwner(strvalue.c_str());
                }
                break;
            case 29:
                pOrder->SetPrice((pVal->g_un).un_double);
                break;
            case 30:
                if (pVal->arr_len)
                {
                    pOrder->SetSecType((pVal->g_un).un_array[0]);
                    if (CDestsManager::getInstance().IsOptionOnFuture(atoi(pOrder->Exchange()), pOrder->SecType()))
                    {
                        pOrder->SetSecIndex(ROMVALUE::OPTIONONFUTURE);
                    }
                }
                break;
            case 31:
                pOrder->SetSide((pVal->g_un).un_int);
                break;
            case 32:
                pOrder->SetStatus((pVal->g_un).un_int);
                //edge case
                //if (pOrder->Status() == ROMVALUE::STATUS_ROMREPLACEPENDING)
                //{
                //    pOrder->SetReplacePending(true);
                //    pOrder->SetLoadedReplacePending(true);
                //}
                //else if (pOrder->Status() == ROMVALUE::STATUS_ROMCANCELPENDING)
                //{
                //    pOrder->SetCancelPending(true);
                //}
                break;
            case 33:
                {
                    char tmp[100];
                    sprintf(tmp, "%f", (pVal->g_un).un_double);
                    pOrder->SetStopPrice(tmp);
                }
                break;
            case 34:
                pOrder->SetStrikePrice((pVal->g_un).un_double);
                break;
            case 35:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetSymbol(strvalue.c_str());
                }
                break;
            case 36:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetClientTag(strvalue.c_str());
                }
                break;
            case 37:
                pOrder->SetTIF((pVal->g_un).un_int);
                break;
            case 38:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetTradeFor(strvalue.c_str());
                }
                break;
            case 39:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetTrader(strvalue.c_str());
                }
                break;
            case 40:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetUnderlying(strvalue.c_str());
                }
                break;
            case 41:
                if (pVal->arr_len)
                {
                    pOrder->SetExecInst((pVal->g_un).un_array[0]);
                }
                break;
            case 42:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetParentRomTag(strvalue.c_str());
                }
                break;
            case 43:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetSecID(strvalue.c_str());
                }
                break;
            case 44:
                pOrder->SetCplxType((pVal->g_un).un_int);
                break;
            case 45:
                {
                    char pegoffset[10];
                    sprintf(pegoffset, "%f", (pVal->g_un).un_double);
                    pOrder->SetPegOffset(pegoffset);
                }
                break;
            case 46:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetSecDef(strvalue.c_str());
                }
                break;
            case 47:
                if (pVal->arr_len)
                {
                    if (CUtility::IsOptionType(pOrder->SecType()))
                    {
                        strvalue.assign(&((pVal->g_un).un_array[8]), 2);
                        pOrder->SetMaturityDay(strvalue.c_str());
                    
                        strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                        char yymm[7];
                        sprintf(yymm, "%s%s", 
                                strvalue.substr(0, 4).c_str(),
                                strvalue.substr(5, 2).c_str());
                        pOrder->SetExpYearMonth(yymm);
                    }
                    else if (CUtility::IsFutureType(pOrder->SecType()))
                    {
                        strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                        char yymm[7];
                        sprintf(yymm, "%s%s", 
                            strvalue.substr(0, 4).c_str(),
                            strvalue.substr(5, 2).c_str());
                        pOrder->SetExpYearMonth(yymm);
                    }
                }
                break;
            case 48:
                pOrder->SetAlgoType((pVal->g_un).un_int);
                break;
            case 49:
                pOrder->SetNextExecId((pVal->g_un).un_int + 1);
                break;
            case 50:
                if (pVal->arr_len)
                {
                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                    pOrder->SetParentClientTag(strvalue.c_str());
                }
                break;
            
            default:
                break;
            }
        }
        
        free(colE);
        gendb_destroy_v(pVal);
        cnt++;
    }
}

bool CThreadRomOrdersNew::LoadOrdersFromDB()
{
    //need to take care of CoveredSpread balance installation!!!
    bool rc = true;
    queue_t results = create_queue();
    
    std::string errStr;
    std::string romInstance = CConfig::getInstance().InstanceName();
   
    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    databuf_write(pBuffer, "SELECT A.avgprice, A.callput, A.capacity, A.clearingaccount, "
                            "A.clearingid, A.clienttime, A.cmtaid, A.cumqty, A.destid, "
                            "A.destrouteid, A.exdest, A.orderexpiresdate, A.firm, A.giveup, A.instructions, "
                            "A.leavesqty, A.localacct, A.maxfloor, A.minqty, A.mmacct, A.mmexchange,"
                            "A.contract_size, A.omtag, A.openclose, A.orderexpiresdate, A.romexchangetag, "
                            "A.ordertype, A.originalshares, A.owner, A.price, A.sectype, A.side, "
                            "A.status, A.stopprice, A.strikeprice, A.symbol, A.clienttag, A.tif, A.tradefor, "
                            "A.trader, A.underlying, A.execinst, A.omparenttag, A.secid, A.cplxtype, "
                            "A.pegoffset, A.secdef, A.expiredate, A.algotype, ");
                            
    databuf_t* pBuffer1 = CBuffStore::getInstance().Get();
    databuf_write(pBuffer1, "A.status_count, A.clientparenttag FROM orders A, "
                            "(SELECT omTag, max(status_count) AS status_count "
                            "FROM Orders WHERE cplxtype IS null OR cplxtype = %d OR "
                            "(cplxtype = %d AND omparenttag IS NOT null) GROUP BY omTag) B "
                            "WHERE B.omTag=A.omTag AND B.status_count=A.status_count "
                            "AND A.status NOT IN (4, 8, 2, 12) AND A.omname='%s'",
                            ROMVALUE::CPLXTYPE_SPREAD, ROMVALUE::CPLXTYPE_LEG, romInstance.c_str());
                            
    std::string sqlStr;
    sqlStr.assign(pBuffer->buffer, databuf_unread(pBuffer));
    sqlStr.append(pBuffer1->buffer, databuf_unread(pBuffer1));

    printf("Starting querying live order from DB\n");

    queue_t row = NULL;
    element_t rowE; 

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        std::map<std::string, tUserDefinedSpread*> theSpreads;
        CRomMap<long> RomTagToRatio;
        std::set<CRomOrderNew*> children;
        
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            
            CRomOrderNew* pOrder = CStore<CRomOrderNew>::getInstance().Get();
            pOrder->SetLoadedReplacePending(true);  //replacing fields are lost after loading from DB!
            
            SetOrder(pOrder, row);

            if (pOrder->Multiplier() == 0)
            {
                double multiplier = (pOrder->SecType() == ROMVALUE::SECTYPE_OPTION)? 100 : 1;
                
                if (pOrder->SecType() == ROMVALUE::SECTYPE_FUTURE)
                {
                    std::string key;
                    size_t ls = get_symbol_len(const_cast<char*>(pOrder->Symbol()), strlen(pOrder->Symbol()));
                    key.assign(pOrder->Symbol(), ls);
                    m_ProductMultiplier.Get(key, multiplier);
                }

                pOrder->SetMultiplier(multiplier);
            }
            
            pOrder->MakeClearinSecKey();
            pOrder->SetNeedPosMgr();
            
            if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SPREAD)
            {
                //build spread map
                tUserDefinedSpread* theSpread = new tUserDefinedSpread;
                theSpread->body = pOrder;
                theSpreads.insert(std::map<std::string, tUserDefinedSpread*>::value_type(pOrder->RomTag(), theSpread));
            }

            if (strlen(pOrder->ParentRomTag()) != 0)
            {
                //collect children
                children.insert(pOrder);
            }
            else
            {
                if (pOrder->CplxType() != ROMVALUE::CPLXTYPE_SPREAD)
                {
                    UpdateBalanceFromLiveOrder(*pOrder);
                }
                StoreOrder(pOrder);
                if (pOrder->NeedPosMgr() && 
                    pOrder->Side() != ROMVALUE::SIDE_BUY)
                {
                    InstallPosFromSSLiveOrderLeave(pOrder);
                }			
            }

            free(rowE);
            destroy_queue(row);
        }

        CRomOrderNew* pChild = NULL;
        std::set<CRomOrderNew*>::iterator itt = children.begin();
        std::map<std::string, tUserDefinedSpread*>::iterator bodyIt;
        tUserDefinedSpread* theSpread = NULL;
        while (itt != children.end())
        {
            //connect children to their parents
            pChild = *itt;
            bodyIt = theSpreads.find(pChild->ParentRomTag());
            if (bodyIt != theSpreads.end())
            {
                theSpread = bodyIt->second;
                theSpread->legs.push_back(pChild);
                StoreOrder(pChild);
            }
            
            itt++;
        }

        //need to get the body side before possible flip and
        //make it as side
        //call UpdateBalanceFromLiveUserDefinedSpreadOrder
        //and then reset the correct side back!!!
        std::map<std::string, short> spreadOriginalSide;
        LoadSpreadOriginalSide(spreadOriginalSide);

        bodyIt = theSpreads.begin();
        std::map<std::string, short>::iterator iiit;
        short oldside, newside;
        while (bodyIt != theSpreads.end())
        {
            theSpread = bodyIt->second;
            iiit = spreadOriginalSide.find(theSpread->body->RomTag());
            if (iiit != spreadOriginalSide.end())
            {
                oldside = iiit->second;
                newside = theSpread->body->Side();
                theSpread->body->SetSide(oldside);
                UpdateBalanceFromLiveUserDefinedSpreadOrder(*(theSpread->body), theSpread->legs);
                theSpread->body->setSide(newside);
            }
            delete theSpread;
            bodyIt++;
        }
        
        //send_debug_message("Finished querying live order from DB\n");
        printf("Finished querying live order from DB\n");
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
        rc = false;
    }
    
    CBuffStore::getInstance().ReturnB(pBuffer1);

    destroy_queue(results);

    if (rc)
    {
        pBuffer1 = CBuffStore::getInstance().Get();
        databuf_write(pBuffer1, "C.status_count, A.clientparenttag FROM orders A, "
                                "(SELECT omTag, max(status_count) AS status_count "
                                "FROM Orders where cplxtype = %d GROUP BY omTag) B, "
                                "(SELECT omTag, max(status_count) AS status_count "
                                "FROM Orders GROUP BY omTag) C "
                                "WHERE C.omTag=A.omTag AND A.omTag=B.omTag AND A.status_count=B.status_count "
                                "AND A.status NOT IN (4,8) AND A.omname='%s'",
                                ROMVALUE::CPLXTYPE_PRODUCT_SPREAD,
                                romInstance.c_str());
                                
        sqlStr.assign(pBuffer->buffer, databuf_unread(pBuffer));
        sqlStr.append(pBuffer1->buffer, databuf_unread(pBuffer1));

        executor = CConfig::getInstance().dbexecutor();
        results = create_queue();
        if (executor->Execute(sqlStr, NULL, 0, results, errStr))
        {
            queue_t row = NULL;
            while (is_empty(results) == 0)
            {
                //int cnt = 0;
                rowE = pop_front(results);
                row = (queue_t)(rowE->d);
                
                CRomOrderNew* pOrder = CStore<CRomOrderNew>::getInstance().Get();
                pOrder->SetLoadedReplacePending(true);  //replacing fields are lost after loading from DB!
                
                SetOrder(pOrder, row);
                if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD &&
                    strlen(pOrder->Instruction()) > 0)
                {
                    pOrder->CreateCoveredSpread();
                }
                StoreOrder(pOrder);
                UpdateBalanceFromLiveOrder(*pOrder);
                if (pOrder->NeedPosMgr() && 
                    pOrder->Side() != ROMVALUE::SIDE_BUY)
                {
                    InstallPosFromSSLiveOrderLeave(pOrder);
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
            rc = false;
        }
        CBuffStore::getInstance().ReturnB(pBuffer1);
        CBuffStore::getInstance().ReturnB(pBuffer);
        destroy_queue(results);
    }

    return rc;
}

//void CThreadRomOrdersNew::InstallPosFromLiveOrder(CRomOrderNew* pOrder, CRomMap<tPosUpdate*>& cumMap)
void CThreadRomOrdersNew::InstallPosFromSSLiveOrderLeave(CRomOrderNew* pOrder)
{
    long balance = pOrder->Leave();

    pOrder->UpdatePosOnly(balance);

    if (CPositionManager::getInstance().FoundEasySymbol(pOrder->Symbol()) == false)
    {
        pOrder->UpdateLendingBalanceOnly(balance);
    }
}
/*
bool CThreadRomOrdersNew::FillParent(const std::string& parentRomTag, long lastShares, tBalanceUpdate& BUpdate, CRomOrderNew* pParent)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(parentRomTag, ptOrder);
    if (rc)
    {
        ptOrder->UpdateFromChildFill(lastShares, BUpdate, pParent);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}
*/
void CThreadRomOrdersNew::UpdateLegsReplace(const std::string& owner, const std::vector<CCsv*>& legs, long shareChange)
{
    CCsv* pCsv = NULL;
    CThreadOrderNew* ptOrder = NULL;
    std::string clienttag = owner;
    std::string tag;

    pthread_rwlock_rdlock(&m_lock);

    for (size_t i = 0; i < legs.size(); i++)
    {
        pCsv = legs[i];

        if (pCsv->GetAt(ROMFIELDS::CSV_TAG, tag))
        {
            std::string theTag;
            MakeTheClientTag(theTag, owner.c_str(), tag.c_str());
            
            if (m_ClientTagToOrder.Get(theTag, ptOrder))
            {
                ptOrder->SetShareChange(shareChange, tag);
                pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, tag);
            }
        }
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrdersNew::StoreLiveOrders(const std::set<databuf_t*>& bufs)
{
    std::set<databuf_t*>::iterator it = bufs.begin();
    databuf_t* pBuf = NULL;
    CRomMap<CRomOrderNew*> parents;
    CRomMap<long> RomTagToRatio;
    std::set<std::string> parentUpdatCandidatorSelected;
    std::set<CRomOrderNew*> children;

    while (it != bufs.end())
    {
        pBuf = *it;
        CCsv* pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(pBuf->buffer, databuf_unread(pBuf));
        CRomOrderNew* pOrder = CStore<CRomOrderNew>::getInstance().Get();
        pOrder->SingleSet(*pCsv);
        pOrder->SetLoadedReplacePending(true); //replacing fields are lost after loading from OATS!
        
        //restore CancelPending/ReplacePending
        //if (pOrder->Status() == ROMVALUE::STATUS_ROMREPLACEPENDING )
        //{
        //    pOrder->SetReplacePending(true);
        //}
        //else if (pOrder->Status() == ROMVALUE::STATUS_ROMCANCELPENDING)
        //{
        //    pOrder->SetCancelPending(true);
        //}

        CStore<CCsv>::getInstance().Return(pCsv);
        //StoreOrder(pOrder);

        //if (pOrder->CplxType() != ROMVALUE::CPLXTYPE_SPREAD &&
        //    strlen(pOrder->ParentRomTag()) == 0)
        //{
        //    UpdateBalanceFromLiveOrder(*pOrder);
        //}

        if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SPREAD )
        {
            parents.Add(pOrder->RomTag(), pOrder);
        }

        if (strlen(pOrder->ParentRomTag()) != 0)
        {
            children.insert(pOrder);

            long ratio = 0;

            if (RomTagToRatio.Get(pOrder->ParentRomTag(), ratio) == true)
            {
                ratio += pOrder->Shares();
                RomTagToRatio.Update(pOrder->ParentRomTag(), ratio);
            }
            else
            {
                ratio = pOrder->Shares();
                RomTagToRatio.Add(pOrder->ParentRomTag(), ratio);
            }

            if (parentUpdatCandidatorSelected.find(pOrder->ParentRomTag()) == parentUpdatCandidatorSelected.end())
            {
                pOrder->SetUpdateParent(true);
                parentUpdatCandidatorSelected.insert(pOrder->ParentRomTag());
            }
        }
        else
        {
            if (pOrder->CplxType() != ROMVALUE::CPLXTYPE_SPREAD)
            {
                UpdateBalanceFromLiveOrder(*pOrder);
            }
            StoreOrder(pOrder);
            if (pOrder->NeedPosMgr())
            {
                pOrder->UpdatePosOnly(pOrder->Leave());

                if (CPositionManager::getInstance().FoundEasySymbol(pOrder->Symbol()) == false)
                {
                    pOrder->UpdateLendingBalanceOnly(pOrder->Leave());
                }
            }
        }
        it++;
    }

    std::list<CRomMap<long>::tKeyValuePair> keyvalues;
    RomTagToRatio.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<long>::tKeyValuePair>::iterator itt = keyvalues.begin();
    CRomOrderNew* pParent = NULL;

    while (itt != keyvalues.end())
    {
        if (parents.Get(itt->key, pParent) == true) // &&
            //pParent->SecType() != ROMVALUE::SECTYPE_COMPLEX)
        {
            pParent->SetRatio(itt->value);
            UpdateBalanceFromLiveOrder(*pParent);
        }

        itt++;
    }

    CRomOrderNew* pChild = NULL;
    std::set<CRomOrderNew*>::iterator ittt = children.begin();
    while (ittt != children.end())
    {
        //It is not guaranteed that exchange will send ROM reject for all the legs
        //DB or OATS may have legs live while body has already gone!!
        pChild = *ittt;
        if (parents.Find(pChild->ParentRomTag()) == true)
        {
            StoreOrder(pChild);
        }

        ittt++;
    }
    
    /*if (CConfig::getInstance().PosMgrEnabled() &&
        CConfig::getInstance().IsFirstRun())
    {
        InstallLendingBalanceFromCurrentSessionDeadOrderCum();
    }*/
}

void CThreadRomOrdersNew::UpdateBalanceFromLiveOrder(const CRomOrderNew& order)
{
    if (order.Leave() > 0)
    {
        tKeyedBalanceUpdate update;
        memset(&(update.update), 0, sizeof(update.update));
        update.update.secIndex = order.SecIndex();
        update.clearing_sec = order.ClearingSecKey();
        update.clract = order.ClearingAccount();
        update.update.sideIndex = order.SideIndex();
        update.update.orderChange = 1;
        update.update.shareChange = order.Leave() * order.Ratio();
        //update.username = order.Owner();
        update.username = order.Trader();
        
        if (CRomOrderNew::IsLimitType(order.Type()))
        {
            update.update.dollarChange = order.SideSign() * order.Multiplier() * order.CurrencyRate() * order.Leave() * order.Price();
        }
        else
        {
            if (order.NeedEquityMidPrice())
            {
                update.update.dollarChange = order.SideSign() * order.Multiplier() * order.CurrencyRate() * order.Leave() * order.mPrice();
            }
            else
            {
                update.update.dollarChange = 0;
            }
        }

        if (ROMVALUE::FUTURE == update.update.secIndex)
        {
            update.product = order.Product();
        }

        if (order.CoveredSpread())
        {
            CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(update, const_cast<tCoveredSpread*>(order.CoveredSpread()), false);
        }
        else
        {
            CClientProfileManagerNew::getInstance().UpdateBalance(update);
        }
    }
}

void CThreadRomOrdersNew::SetRatio(const std::string& romTag, long value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetRatio(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrdersNew::SetCancelPending(const std::string& romTag, bool value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetCancelPending(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrdersNew::SetReplacePending(const std::string& romTag, bool value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetReplacePending(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}

bool CThreadRomOrdersNew::SendOatsMsg(CCsv* pCsv)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrderNew* ptOrder = NULL;
    std::string romTag;

    pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SendOatsMsg(pCsv);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CThreadRomOrdersNew::MakeOrderReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, " ");
    msg.SetAt(ROMFIELDS::CSV_AVG_PRICE, "0");
    msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, "0");
    msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, "0");

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadRomOrdersNew::MakeOrderReject(CCsv& msg, const CRomOrderNew& order, const std::string& reason)
{
    order.GetCsv(msg);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadRomOrdersNew::MakeCancelReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadRomOrdersNew::MakeReplaceReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

bool CThreadRomOrdersNew::ProcessSingleOrder(CCsv* pCsv, CClientConnection* pCon)
{
    std::string err = "";
    std::string owner, omTime;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    pCsv->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    std::string romtag;
    GetNextRomTag(owner, romtag);
    pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
    //std::string routeID, destID;
    //pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
    //CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
    //pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

    CRomOrderNew* pOrder = CStore<CRomOrderNew>::getInstance().Get();
   
    bool rejected = false;
  
    pOrder->SingleSet(*pCsv);
    std::string sprice;
    if (CRomOrderNew::IsLimitType(pOrder->Type()) &&
        pCsv->GetAt(ROMFIELDS::CSV_PRICE, sprice))
    {
        if (CUtility::IsValidDartSDouble(sprice) == false)
        {
            pOrder->SetPrice(0);  //otherwise db will have strange value!!!
            err.assign("Invalid Price:");
            err.append(sprice);
            rejected = true;
        }
    }
            
    if (false == rejected)
    {
        if (pOrder->SecIndex() == ROMVALUE::FUTURE)
        {
            //if (CConfig::getInstance().FutureProductLimitEnabled())
            //{
                std::string key;
                size_t ls = get_symbol_len(const_cast<char*>(pOrder->Symbol()), strlen(pOrder->Symbol()));
                key.assign(pOrder->Symbol(), ls);
                double multiplier = 1;
                if (m_ProductMultiplier.Get(key, multiplier))
                {
                    pOrder->SetMultiplier(multiplier);
                }
            //}
            
            if (pOrder->Multiplier() == 0)
            {
                //pOrder->SetMultiplier(1);//for now
                //reject order when ready
                err.assign("multiplier is missing for future order");
                rejected = true;
            }
        }
        else if (pOrder->SecIndex() == ROMVALUE::OPTIONONFUTURE)
        {
            //get underlying, if not found get symbol later!
            if (pOrder->Multiplier() == 0)
            {
                err.assign("multiplier is missing for option on future order");
                rejected = true;
            }
        }
        
        if (false == rejected)
        {
            pOrder->SetCurrencyRate(1);

            if (pOrder->NeedPosMgr())
            {
                if (CPositionManager::getInstance().PosRecapFinished() == false)
                {
                    rejected = true;
                    err.assign("PositionManager is not ready");
;               }
                else
                {
                    if (pOrder->Side() == ROMVALUE::SIDE_SELL ||
                        pOrder->Side() == ROMVALUE::SIDE_SHORT)
                    {
                        int side_before = pOrder->Side();

                        if (CPositionManager::getInstance().ProcessSSOrder(pOrder, err) == false)
                        {
                            rejected = true;
                        }
                        else
                        {
                            pCsv->SetAt(ROMFIELDS::CSV_SIDE, pOrder->Side()); //in case side has been fliped!
                            if (side_before != ROMVALUE::SIDE_SHORT &&
                                pOrder->Side() == ROMVALUE::SIDE_SHORT)
                            {
                                pCsv->SetAt(ROMFIELDS::CSV_SHORT_LENDER, pOrder->ShortLender());
                            }
                        }
                    }
                }
            }
            else if (CConfig::getInstance().PosMgrEnabled() == 0 &&
                     CPositionManager::getInstance().FindClr(pOrder->ClearingAccount()))
            {
                err.append("Ronin Trading Clearing Account(");
                err.append(pOrder->ClearingAccount());
                err.append(") can not be used to send order to this ROM");
                rejected = true;
            }
        }
    }
    
    pOrder->QueueOatsMsg(pCsv);
    
    if (true == rejected)
    {
        MakeOrderReject(*pCsv, err);
        pOrder->QueueOatsMsg(pCsv);
        CStore<CRomOrderNew>::getInstance().Return(pOrder);
        CClientsManager::getInstance().Send(owner, *pCsv, err);
    }
    else
    {
        tKeyedBalanceUpdate theChange;
        memset(&(theChange.update), 0, sizeof(theChange.update));
        theChange.update.secIndex = pOrder->SecIndex();
        theChange.username = pOrder->Trader();
        theChange.clearing_sec = pOrder->ClearingSecKey();
        theChange.clract = pOrder->ClearingAccount();
        std::vector<tBalanceUpdate*> balances;
        
        if (!ValidateOrder(*pOrder, pCsv, NULL, balances, err, pCon))
        {
            MakeOrderReject(*pCsv, err);
            pOrder->QueueOatsMsg(pCsv);
            //CStore<CRomOrderNew>::getInstance().Return(pOrder);
            CClientsManager::getInstance().Send(owner, *pCsv, err);
            rejected = true;
        }
        else
        {
            databuf_t* pBuffer = CBuffStore::getInstance().Get();
            pCsv->GetMsgNew(pBuffer);

            StoreOrder(pOrder);

            if (CDestsManager::getInstance().Send(pOrder->Dest(), ROMVALUE::COMMAND_ORDER, pBuffer->buffer, databuf_unread(pBuffer), err) == false)
            {
                CCsv* pReject = CStore<CCsv>::getInstance().Get();
                MakeOrderReject(*pReject, *pOrder, err);
                pReject->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

                //CRomOrderNew::SendOrderRejectEmail(*pOrder, err.c_str());

                SendOatsMsg(pReject);

                CClientsManager::getInstance().Send(owner, *pReject, err);

                //back out the ballance contribution
                for (unsigned short i = 0; i < balances.size(); i++)
                {
                    theChange.update = *(balances[i]);
                    delete balances[i];
                    theChange.update.orderChange *= -1;
                    theChange.update.shareChange *= -1;
                    theChange.update.dollarChange *= -1;
                    if (ROMVALUE::FUTURE == theChange.update.secIndex)
                    {
                        theChange.product = pOrder->Product();
                    }
                    CClientProfileManagerNew::getInstance().UpdateBalance(theChange);
                }
                RemoveOrder(pOrder->RomTag());
                //CStore<CRomOrderNew>::getInstance().Return(pOrder);
                CStore<CCsv>::getInstance().Return(pReject);
                rejected = true;
            }

            CBuffStore::getInstance().ReturnB(pBuffer);
        }
        
        if (rejected)
        {
            if (pOrder->NeedPosMgr() &&
                pOrder->Side() != ROMVALUE::SIDE_BUY)
            {
                //CPositionManager::getInstance().ProcessSSOrderReject(pOrder);
                pOrder->UpdatePos(pOrder->Leave());
            }
            
            CStore<CRomOrderNew>::getInstance().Return(pOrder);
        }
    }
    
    return true;
}



bool CThreadRomOrdersNew::ProcessUserDefinedSpreadOrder(CCsv* pBody, std::vector<CCsv*>* pLegs, CClientConnection* pCon)
{
    bool sent = false;
    bool rejected = false;
    std::string owner, error, romtag, omTime;
    CRomOrderNew* pOrder = NULL;

    pBody->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    pBody->GetAt(ROMFIELDS::CSV_OWNER, owner);
    GetNextRomTag(owner, romtag);
    pBody->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

    pOrder = CStore<CRomOrderNew>::getInstance().Get();
       
    pOrder->SingleSet(*pBody);
    if (pLegs->size() == 0)
    {
        error.assign("No Legs");
        rejected = true;
    }
    else
    {
        std::string sprice;
        if (CRomOrderNew::IsLimitType(pOrder->Type()) &&
            pBody->GetAt(ROMFIELDS::CSV_PRICE, sprice))
        {
            if (CUtility::IsValidDartSDouble(sprice) == false)
            {
                pOrder->SetPrice(0);  //otherwise db will have strange value!!!
                error.assign("Invalid Price:");
                error.append(sprice);
                rejected = true;
            }
        }
    }
    
    if (false == rejected)
    {
        if (pOrder->SecIndex() == ROMVALUE::FUTURE)
        {
            //if (CConfig::getInstance().FutureProductLimitEnabled())
            //{
                std::string key;
                size_t ls = get_symbol_len(const_cast<char*>(pOrder->Symbol()), strlen(pOrder->Symbol()));
                key.assign(pOrder->Symbol(), ls);
                double multiplier = 1;
                if (m_ProductMultiplier.Get(key, multiplier))
                {
                    pOrder->SetMultiplier(multiplier);
                }
            //}

            if (pOrder->Multiplier() == 0)
            {
                rejected = true;
                error.assign("missing multiplier for future order");
            }
        }
    }
    
    std::vector<tKid> childList;
    tKeyedBalanceUpdate BUpdate;
    std::vector<tBalanceUpdate*> balances;
    
    if (false == rejected)
    {
        pOrder->SetCurrencyRate(1);

        //std::vector<tKid> childList;
        CCsv* pM = NULL;
        bool foundOpt = false, foundEqt = false;
       
        for (size_t i = 0; i < pLegs->size(); i++)
        {
            pM = (*pLegs)[i];
            pM->SetAt(ROMFIELDS::CSV_OWNER, pOrder->Owner());
            pM->SetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, pOrder->RomTag());
            pM->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, ROMVALUE::CPLXTYPE_LEG);
            pM->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
           
            GetNextRomTag(owner, romtag);
            pM->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

            CRomOrderNew* pChild = CStore<CRomOrderNew>::getInstance().Get();

            pChild->SingleSet(*pM);
            pM->SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, pChild->OriginalShares());
            
            if (pChild->SecType() == ROMVALUE::SECTYPE_OPTION)
            {
                foundOpt = true;
                pOrder->SetSecType(ROMVALUE::SECTYPE_OPTION);
                pOrder->SetMultiplier(100);
                pBody->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, ROMVALUE::SECTYPE_OPTION);
            }
            else if (pChild->SecType() == ROMVALUE::SECTYPE_EQUITY)
            {
                foundEqt = true;
            }

            pChild->SetRatio(pChild->Shares());
            //pChild->SetRatio(pOrder->Shares());
            pChild->SetRomTag(romtag.c_str());
            pChild->InheritParent(pOrder);

            tKid kid = {pChild, pM};
            childList.push_back(kid);
        }

        //handle position validation here
        //std::vector<CRomOrderNew*> legpos;

        pOrder->QueueOatsMsg(pBody);
        int chldsize = childList.size();
        for (int i = 0; i < chldsize; i++)
        {
            childList[i].pOrder->QueueOatsMsg(childList[i].pCsv);
        }

        if (foundOpt == true && foundEqt == true) //covered spread
        {
            pOrder->CreateCoveredSpread(*pLegs);

            //pOrder->SetMixedSpread(true);
            //pOrder->SetMultiplier(1);
        }

        //tKeyedBalanceUpdate BUpdate;
        memset(&(BUpdate.update), 0, sizeof(BUpdate.update));
        BUpdate.update.secIndex = pOrder->SecIndex();
        BUpdate.username = pOrder->Trader();
        BUpdate.clearing_sec = pOrder->ClearingSecKey();
        BUpdate.clract = pOrder->ClearingAccount();

        if (ROMVALUE::FUTURE == BUpdate.update.secIndex)
        {
            BUpdate.product = pOrder->Product();
        }

        //databuf_t* pBuffer = NULL;

        if (rejected == false)
        {
            rejected = !ValidateOrder(*pOrder, pBody, pLegs, balances, error, pCon);
        }
    }
    
    databuf_t* pBuffer = NULL;
    
    if (rejected)
    {
        MakeOrderReject(*pBody, error);
        pBuffer = CBuffStore::getInstance().Get();
        pBody->GetMsgNew(pBuffer);
        CClientsManager::getInstance().Send(owner, pBuffer->buffer, databuf_unread(pBuffer), error);
        CBuffStore::getInstance().ReturnB(pBuffer);

        pOrder->QueueOatsMsg(pBody);

        for (size_t i = 0; i < pLegs->size(); i++)
        {
            MakeOrderReject(*(childList[i].pCsv), error);
            childList[i].pOrder->QueueOatsMsg(childList[i].pCsv);
        }
    }
    else
    {
        StoreOrder(pOrder);

        if (childList.size())
        {
            childList[0].pOrder->SetUpdateParent(true);
        }

        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        databuf_t* pLegBuffer = NULL;

        pBody->getMsgNew(pBuffer);
        databuf_memcpy(pBuffer, ROMVALUE::LEG_SEPRATOR.c_str(), ROMVALUE::LEG_SEPRATOR.size());

        //pack the whole csv string
        for (size_t i = 0; i < pLegs->size(); i++)
        {
            //pOrder->AddLeg(childList[i].pOrder);

            StoreOrder(childList[i].pOrder);

            pLegBuffer = CBuffStore::getInstance().Get();
            (*pLegs)[i]->getMsgNew(pLegBuffer);
            databuf_memcpy(pBuffer, pLegBuffer->buffer, databuf_unread(pLegBuffer));
            CBuffStore::getInstance().ReturnB(pLegBuffer);
            databuf_memcpy(pBuffer, ROMVALUE::LEG_SEPRATOR.c_str(), ROMVALUE::LEG_SEPRATOR.size());
        }

        databuf_write(pBuffer, "%c", ROMFIELDS::ENDER);

        sent = CDestsManager::getInstance().Send(pOrder->Dest(), ROMVALUE::COMMAND_ORDER, pBuffer->buffer, databuf_unread(pBuffer), error);
        CBuffStore::getInstance().ReturnB(pBuffer);

        if (false == sent)
        {
            for (size_t i = 0; i < pLegs->size(); i++)
            {
                MakeOrderReject(*(childList[i].pCsv), error);
                childList[i].pOrder->QueueOatsMsg(childList[i].pCsv);
                /*
                if (childList[i].pOrder->NeedPosMgr() &&
                    childList[i].pOrder->Side() != ROMVALUE::SIDE_BUY)
                {
                    CPositionManager::getInstance().ProcessSSOrderReject(childList[i].pOrder);
                }
                */
            }

            CCsv* pReject = CStore<CCsv>::getInstance().Get();
            MakeOrderReject(*pReject, *pOrder, error);
            pReject->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            pOrder->QueueOatsMsg(pReject);

            CClientsManager::getInstance().Send(owner, *pReject, error);
            
            for (unsigned short i = 0; i < balances.size(); i++)
            {
                BUpdate.update = *(balances[i]);
                delete balances[i];
                BUpdate.update.orderChange = -1;
                BUpdate.update.shareChange *= -1;
                BUpdate.update.dollarChange *= -1;

                CClientProfileManagerNew::getInstance().UpdateBalance(BUpdate);
            }
        }
    }

    if (sent != true)
    {
        RemoveOrder(pOrder->RomTag());
                
        CStore<CRomOrderNew>::getInstance().Return(pOrder);

        for (size_t i = 0; i < pLegs->size(); i++)
        {
            RemoveOrder(childList[i].pOrder->RomTag());
            CStore<CRomOrderNew>::getInstance().Return(childList[i].pOrder);
        }
    }

    return true;
}

bool CThreadRomOrdersNew::ValidateSendReplace(CCsv* pReplace, CClientConnection* pCon)
{
    bool rc = true;
    CThreadOrderNew* ptOrder = NULL;
    std::string error;

    //if (pCon && const_cast<CClientConnection*>(pCon)->hitLimit())
    //{
    //    error = CConfig::getInstance().clientMsgLimitError();
    //    rc = false;
    //}
    //else
    //{
        std::string tag, owner;
        pReplace->GetAt(ROMFIELDS::CSV_OWNER, owner);
        pReplace->GetAt(ROMFIELDS::CSV_TAG, tag);

        pthread_rwlock_rdlock(&m_lock);

        std::string theTag;
        MakeTheClientTag(theTag, owner.c_str(), tag.c_str());

        rc = m_ClientTagToOrder.Get(theTag, ptOrder);
        if (false == rc)
        {
            error = "Order not found";
        }
    //}

        if (rc)
        {
            long newQty = 0;
            if (pReplace->GetAt(ROMFIELDS::CSV_SHARES, newQty) &&
                newQty <= 0)
            {
                error = "new order quantity has to be larger than zero";
                rc = false;
            }
        }
        std::string sprice;
        
    if (rc == true && (pReplace->GetAt(ROMFIELDS::CSV_PRICE, sprice))) 
    { 
        
        if (CUtility::IsValidDartSDouble(sprice))
        {
            CRomOrderNew dummy;
            ptOrder->GetOrder(&dummy);
            if(dummy.SecIndex() == ROMVALUE::EQUITY) 
            {
                char price[50];
                price[0] = '\0';
                pReplace->GetAt(ROMFIELDS::CSV_PRICE, price, sizeof(price));

                if (CUtility::IsEquityPriceDecimalValid(price, error) == false)
                {
                    rc = false;
                }
            }
        }
        else
        {
            error.assign("Invalid Price:");
            error.append(sprice);
            rc = false;
        }
    }


    if (false == rc)
    {
        pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        pReplace->SetAt(ROMFIELDS::CSV_TEXT, error);
        pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
        SendOatsMsg(pReplace);
    }
    else
    {
        rc = ptOrder->ProcessReplace(pReplace, pCon);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrdersNew::ValidateSendCancel(CCsv* pCancel, bool ownercancel, CClientConnection* pCon)
{
    bool rc = true;
    std::string actTag;
    CThreadOrderNew* ptOrder = NULL;

    //if (pCon && const_cast<CClientConnection*>(pCon)->hitLimit())
    //{
    //    error = CConfig::getInstance().clientMsgLimitError();
    //    rc = false;
    //}
    //else
    //{
        std::string owner;
        pCancel->GetAt(ROMFIELDS::CSV_OWNER, owner);

        pthread_rwlock_rdlock(&m_lock);

        if (ownercancel)
        {
            std::string theTag, tag, owner;
            pCancel->GetAt(ROMFIELDS::CSV_OWNER, owner);
            pCancel->GetAt(ROMFIELDS::CSV_TAG, tag);
            MakeTheClientTag(theTag, owner.c_str(), tag.c_str());

            rc = m_ClientTagToOrder.Get(theTag, ptOrder);
        }
        else
        {
            std::string romTag;
            pCancel->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);
            rc = m_RomTagToOrder.Get(romTag, ptOrder);
        }

    //}
    if (false == rc)
    {
        if (ownercancel)
        {
            std::string error = "Order not found";
            pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            pCancel->SetAt(ROMFIELDS::CSV_TEXT, error);
            pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
        }
        else
        {
            std::string message;
            pCancel->GetMsg(message);
            std::string errMsg = "Order Not Found: ";
            errMsg.append(message);
            CUtility::SendRollingLogMessage(CConfig::getInstance().InstanceName(), errMsg);
        }
    }
    else
    {
        rc = ptOrder->ProcessCancel(pCancel, actTag, ownercancel, pCon);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrdersNew::ProcessClientCancel(CCsv* pCsv, CClientConnection* pCon)
{
    std::string err;
    std::string owner;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    //COatsMsg* pOatsMsg = NULL;
    int ownercancel = 0;
    pCsv->GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);

    if (ValidateSendCancel(pCsv, ownercancel, pCon) == false)
    {
        if (ownercancel)
        {
            CClientsManager::getInstance().Send(owner, *pCsv, err);
        }
        else
        {
            struct message m;
            m.data = const_cast<char*>(err.c_str());
            m.len = err.length();
            m.name = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
            m.name_len = CConfig::getInstance().InstanceName().length();
            dart_rolling_log_coming(&m, m.name, m.name_len);
        }
    }

    return true;
}

bool CThreadRomOrdersNew::ProcessClientReplace(CCsv* pBody, CClientConnection* pCon)
{

    std::string owner;
    pBody->GetAt(ROMFIELDS::CSV_OWNER, owner);

    if (ValidateSendReplace(pBody, pCon) == false)
    {
        std::string dummy;
        CClientsManager::getInstance().Send(owner, *pBody, dummy);
    }

    return true;
}

bool CThreadRomOrdersNew::ProcessClientCancels(CCsv* pCsv, bool dayorderonly)
{
    std::set<std::string> tags;

    int ownercancel = 0;
    pCsv->GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
    std::string omTime;
    pCsv->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    BuildCancels(*pCsv, tags, dayorderonly);
    std::set<std::string>::iterator it = tags.begin();

    CCsv* pCancel = CStore<CCsv>::getInstance().Get();
    pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
    pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
    pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    std::string owner;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    pCancel->SetAt(ROMFIELDS::CSV_OWNER, owner);
    pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);

    std::string tag;

    while (it != tags.end())
    {
        tag = *it;
        if (ownercancel)
        {
            pCancel->SetAt(ROMFIELDS::CSV_TAG, tag);
        }
        else
        {
            pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, tag);
        }

        ProcessClientCancel(pCancel, NULL);

        pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
        //CSV_OWNERCANCEL = CSV_OMEXECUTION_TAG!!!
        pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
        pCancel->SetAt(ROMFIELDS::CSV_TEXT, "");

        it++;
    }

    CStore<CCsv>::getInstance().Return(pCancel);

    return true;
}

bool CThreadRomOrdersNew::ProcessDestCancels(const std::string& destID)
{
    std::set<std::string> romTags;

    BuildDestCancels(destID, romTags);
    std::set<std::string>::iterator it = romTags.begin();

    CCsv* pCancel = CStore<CCsv>::getInstance().Get();
    pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
    pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 0);
    char t[32];
    CUtility::GetGmtTimeStr(t, sizeof(t));
    pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, t);

    std::string romTag;

    while (it != romTags.end())
    {
        romTag = *it;
        pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);
        ProcessClientCancel(pCancel);
        it++;
    }

    CStore<CCsv>::getInstance().Return(pCancel);

    return true;
}


bool CThreadRomOrdersNew::ProcessHostUpdate(CCsv* pCsv)
{
    std::string romtag;
    pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

    char omTime[32];
    CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
    pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);

    CThreadOrderNew* ptOrder = NULL;

    pthread_rwlock_rdlock(&m_lock);
    bool unlocked = false;

    if (m_RomTagToOrder.Get(romtag, ptOrder) == false)
    {
        std::string message;

        if (ROMVALUE::STATUS_FILLED == status ||
            ROMVALUE::STATUS_PARTIALLYFILLED == status)
        {
            //send email
            std::string symbol, lastshare, lastprice;
            pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
            pCsv->GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
            pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshare);
            pCsv->GetAt(ROMFIELDS::CSV_LAST_PRICE, lastprice);

            char sbj[256];
            sprintf(sbj, "Order Not Found in %s(RomTag=%s Symbol=%s LastShares=%s LastPrice=%s) ", CConfig::getInstance().Login().c_str(), romtag.c_str(), symbol.c_str(), lastshare.c_str(), lastprice.c_str());
            std::string subject = sbj;
            pCsv->GetMsg(message);
            CUtility::SendBizEMail(subject, message);
        }
        else
        {
            pCsv->GetMsg(message);
        }

        std::string errMsg = "Order Not Found: ";
        errMsg.append(message);

        struct message msg;
        msg.name = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
        msg.name_len = CConfig::getInstance().InstanceName().length();
        msg.data = const_cast<char*>(errMsg.c_str());
        msg.len = errMsg.length();
        dart_rolling_log_coming(&msg, msg.name, msg.name_len);
    }
    else
    {
        char cmd = 'S';
        if (pCsv->GetAt(ROMFIELDS::CSV_COMMAND, cmd) &&
            cmd == ROMVALUE::COMMAND_ZAP)
        {
            if (ptOrder->GetStatus() == ROMVALUE::STATUS_NEW)
            {
                pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
            }
            else
            {
                pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
            }
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Zapped");
            pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        }

        ptOrder->ProcessUpdate(pCsv);

        int leave = 0;
        pCsv->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);

        if (leave < 0)
        {
            //generate email to indicate possible overfill
            std::string symbol;
            pCsv->GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
            char sbj[256];
            sprintf(sbj, "Order may have been overfilled in %s(RomTag=%s Symbol=%s) ", CConfig::getInstance().Login().c_str(), romtag.c_str(), symbol.c_str());
            std::string subject = sbj, dumy="";
            CUtility::SendBizEMail(subject, dumy);
        }

        if (status == ROMVALUE::STATUS_REPLACED)
        {
            if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_NEW_TAG) == false)
            {
                std::string oldTag, newTag, owner;
                pCsv->GetAt(ROMFIELDS::CSV_TAG, oldTag);
                pCsv->GetAt(ROMFIELDS::CSV_NEW_TAG, newTag);
                pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
                pthread_rwlock_unlock(&m_lock);
                unlocked = true;
                std::string theNewTag, theOldTag;
                MakeTheClientTag(theNewTag, owner.c_str(), newTag.c_str());
                MakeTheClientTag(theOldTag, owner.c_str(), oldTag.c_str());
                UpdateClientTag(theOldTag, theNewTag);
            }
        }

        int cplxtype = ptOrder->GetCplxType();

        if ((leave <= 0 &&
            !(status != ROMVALUE::STATUS_ORDERREJECTED &&
              ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == cplxtype)) ||
            (status == ROMVALUE::STATUS_DONEFORDAY && 
             ptOrder->GetTif() != ROMVALUE::TIF_GTC &&
             ptOrder->GetTif() != ROMVALUE::TIF_GTD))
        {
            //The edge case that replace unintentionally makes leave go to
            //zero because partial fills in transit when replace is going out.
            if (ROMVALUE::STATUS_REPLACED == status)
            {
                pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
                pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Zapped");
                ptOrder->ProcessUpdate(pCsv);
                char sbj[256];
                sprintf(sbj, "%s generated Zap for RomTag=%s", CConfig::getInstance().Login().c_str(), romtag.c_str());
                std::string subject = sbj, dumy="Replace unintentionally makes leave go to zero(or below zero) because there were fills in transit while replace was going out";
                CUtility::SendBizEMail(subject, dumy);
            }

            if (false == unlocked)
            {
                pthread_rwlock_unlock(&m_lock);
                unlocked = true;
            }

            CRomOrderNew* pOrder = RemoveOrder(romtag);
            CStore<CRomOrderNew>::getInstance().Return(pOrder);
        }
    }

    if (false == unlocked)
    {
        pthread_rwlock_unlock(&m_lock);
    }

    return true;
}

bool CThreadRomOrdersNew::ValidateOrder(CRomOrderNew& order, CCsv* pCsv, const std::vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& balances, std::string& error, CClientConnection* pCon)
{
    if (pCon)
    {
        if (false == pCon->IncrementMsgCntCheck(order.SecIndex(), error))
        {
            return false;
        }
    }

    //make sure the quantity is larger than 0
    if (order.Shares() <= 0)
    {
        error = "Order quantity has to be larger than zero";
        return false;
    }

    //make sure order tag not duplicated
    CRomOrderNew dummy;
    std::string theTag;
    MakeTheClientTag(theTag, order.Owner(), order.ClientTag());

    if (GetOrderFromClientTag(theTag, &dummy))
    {
        error = "Duplicated order tag:" + theTag;
        return false;
    }

    if (order.ExecInst() == ROMVALUE::ISO)
    {
        error = "ISO order is not allowed by ROM system";
        return false;
    }

    //Short lender
    if (ROMVALUE::SIDE_EXEMPT == order.Side())
    {
        error = "Dart Execution does not support Short Sell Exempt order";
        return false;
    }

    if (order.Side() == ROMVALUE::SIDE_SHORT &&
        strcmp(order.ShortLender(), "") == 0)
    {
        error = "Short Lender(tag 58) can not be empty for short sell or short sell exempt order";
        return false;
    }

    //clearing id
    if (strcmp(order.ClearingId(), "") == 0)
    {
        error = "ClearingID(tag 11) can not be empty";
        return false;
    }

    if (strcmp(order.ClearingAccount(), "") == 0)
    {
        error = "ClearingAccount(tag 12) can not be empty";
        return false;
    }

    //validate routeable exchange code
    std::string destRtId;
    if (CDestsManager::getInstance().GetRouteIdFromDestId(order.Exchange(), destRtId) == false)
        //if (-1 == destRtId)
    {
        error = "Invalid Exchange(tag 13)";
        return false;
    }

    //capacity checking
    char secType = ROMVALUE::SECTYPE_EQUITY;
    CDestsManager::getInstance().GetSecTypeFromRouteId(destRtId, secType);

    if (ROMVALUE::SECTYPE_OPTION == secType &&
        CConfig::getInstance().IsSpecialOptionDest(destRtId) == false)
    {
        if ('B' != order.Capacity() &&
            'S' != order.Capacity() &&
            'X' != order.Capacity() &&
            'C' != order.Capacity())
        {
            error = "Option Order is allowed only for Capacity 'S'(MarketMaker) and Capacity 'B'(BrokerDealerFirm)";
            return false;
        }
    }
    else
    {
        if (CConfig::getInstance().IsSpecialNonOptionDest(destRtId) == false &&
            'A' != order.Capacity())
        {
            error = "NON Option Order is allowed only for Capacity 'A'(Agency)";
            return false;
        }
    }

    if (CMDSClient::NeedValidatePrice(order, false))
    {
        switch(order.SecIndex())
        {
        case ROMVALUE::EQUITY:
            {
                char price[50];
                price[0] = '\0';

                if (pCsv->GetAt(ROMFIELDS::CSV_PRICE, price, sizeof(price)))
                {
                    if (price[0] == '-')
                    {
                        error.assign("invalid price");
                        return false;
                    }
                    else
                    {
                        long intPrice = CUtility::GetIntPrice(price);

                        if (CMDSClient::getInstance().ValidatePrice(order, false, intPrice, !pCsv->IsFieldEmpty(ROMFIELDS::CSV_NBBO), error) == false)
                        {
                            return false;
                        }
                    }
                }
            }
            break;
        case ROMVALUE::OPTION:
            {
                std::string nbbo;
                pCsv->GetAt(ROMFIELDS::CSV_NBBO, nbbo);

                switch (order.CplxType())
                {
                case ROMVALUE::CPLXTYPE_SINGLE:
                    {
                        std::string nbbo;
                        pCsv->GetAt(ROMFIELDS::CSV_NBBO, nbbo);
                        
                        if (CMDSClient::getInstance().ValidateOptionPrice(order, order.Price(), nbbo, error) == false)
                        {
                            return false;
                        }
                    }
                    break;
                case ROMVALUE::CPLXTYPE_SPREAD:
                    /*if (order.CoveredSpread())
                    {
                        if (CMDSClient::getInstance().ValidateOptionPriceUserDefinedCoveredSpread(order, *pLegs, false, error) == false)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (CMDSClient::getInstance().ValidateOptionPriceUserDefinedSpread(order, *pLegs, false, error) == false)
                        {
                            return false;
                        }
                    }*/
                    break;
                case ROMVALUE::CPLXTYPE_PRODUCT_SPREAD:
                    /*if (CMDSClient::getInstance().ValidateOptionPriceProductCoveredSpread(order, nbbo, false, error) == false)
                    {
                        //dumy for now
                    }*/

                    break;
                }
            }
            break;
        }
    }

    //Equity price format check
    if (order.SecIndex() == ROMVALUE::EQUITY &&
        CRomOrderNew::IsLimitType(order.Type()) == true)
    {
        if (order.ValidateSingleLimitPrice(order.Price(), error) == false)
        {
            return false;
        }
        
        char price[50];
        price[0] = '\0';
        pCsv->GetAt(ROMFIELDS::CSV_PRICE, price, sizeof(price));
        
        if (CUtility::IsEquityPriceDecimalValid(price, error) == false)
        {
            return false;
        }
        else
        {
            pCsv->SetAt(ROMFIELDS::CSV_PRICE, price);
            const_cast<CRomOrderNew&>(order).SetPrice(atof(price));
        }
    }
    
    bool rc = false;
    switch(order.CplxType())
    {
    case ROMVALUE::CPLXTYPE_SPREAD:
        rc = CClientProfileManagerNew::getInstance().AllowedToPlaceOrder_UserDefinedSpread(order, pLegs, balances, error);
        break;
    case ROMVALUE::CPLXTYPE_PRODUCT_SPREAD:
        rc = CClientProfileManagerNew::getInstance().AllowedToPlaceOrder_ExchangeDefinedSpread(order, balances, error);
        break;
    default:
        rc = CClientProfileManagerNew::getInstance().AllowedToPlaceOrder(order, balances, error);
        break;
    }
   
    return rc;
}

bool CThreadRomOrdersNew::LoadProductMultiplierMap()
{
    bool rc = false;
    
    m_ProductMultiplier.RemoveAll();
    queue_t results = create_queue();
    //const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    std::string sqlStr;
    char sqlstr[1024];

    strcpy(sqlstr, "SELECT Product, Multiplier "
            "FROM Future");

    sqlStr.assign(sqlstr);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        std::string key;
        rc = true;
        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

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
                        m_ProductMultiplier.Add(key, (pVal->g_un).un_double);
                        break;
                    default:
                        break;
                    }
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
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s\n", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
    return rc;
}

void CThreadRomOrdersNew::InstallLendingBalanceFromCurrentSessionDeadOrderCum()
{
    //std::string yyyymmdd;
    //CUtility::GetCentralDateStrYYYYMMDD(yyyymmdd);
    //int index = yyyymmdd.find('-');
    //yyyymmdd = yyyymmdd.substr(0, index);
    
    //const tDBLogin& db = CConfig::getInstance().db();
    queue_t results = create_queue();

    //char sqlStr[1024];
    std::string errStr;
    std::string sqlStr;
    //OLD code
    //should exclude product type complex order
    //sprintf(sqlStr, "SELECT A.exQty, B.side, B.symbol, B.clearingAccount "
    //        "FROM Executions A, Orders B, ClearingActBillingGroupMap C WHERE "
    //        "A._OrdNum=B._OrdNum AND B.clearingAccount=C.clearingAccount AND C.ronin_trading=1 AND A._timeStamp >= '%s'  AND "
    //        "B.secType='E' AND B.cplxtype is NULL AND B.parentOmTag is NULL AND "
    //        "B.omTag LIKE '%c-%s%c-%c'", 
    //        yyyymmdd.c_str(), '%', CConfig::getInstance().sInstance().c_str(), '%', '%');

    //NEW code
    //Once starting receiving both current and start positions from RDS
    //Retrieve buy fills for Threshold symbols ONLY to knock out some
    //lending balance!
    //Assuming equity does not have GTC
    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    databuf_write(pBuffer, "SELECT A.cumqty, A.side, A.symbol, B.clearingaccount FROM "
            "orders A, clearingactbillinggroupmap B WHERE A.omname='%s' AND "
            "B.ronin_trading=1 AND A.sectype='E' AND A.cumqty > 0 AND A.status in (2, 4, 12) AND "
            "A.clearingaccount=B.clearingaccount", CConfig::getInstance().InstanceName().c_str());

    sqlStr.assign(pBuffer->buffer, databuf_unread(pBuffer));
    CBuffStore::getInstance().ReturnB(pBuffer);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();

    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        element_t rowE;

        while (is_empty(results) == 0)
        {
            tPosUpdate update;
            
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            std::string omTag;
            tPosUpdate posUpdate;
            
            while (is_empty(row) == 0)
            {
                element_t colE = pop_front(row);
                gendb_v* pVal = (gendb_v*)(colE->d);

                if (pVal->type_in_union != DB_NULL)
                {
                    switch(cnt)
                    {
                    case 0:
                        update.qty_change = (pVal->g_un).un_int;
                        break;
                    case 1:
                        update.side = (pVal->g_un).un_int;
                        break;  
                    case 2:
                        if (pVal->arr_len)
                        {
                            update.symbol.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 3:
                        if (pVal->arr_len)
                        {
                            update.clract.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    default:
                        break;
                    }
                }

                free(colE);
                gendb_destroy_v(pVal);

                cnt++;
            }

            //they will be received from RDS so 
            //CPositionManager::getInstance().UpdatePositionOnly(&update)
            //should not be called;
            if (CPositionManager::getInstance().FoundEasySymbol(update.symbol) == false)
            {
                if (update.side != ROMVALUE::SIDE_BUY)
                {
                    if (CPositionManager::getInstance().FoundThresholdSymbol(update.symbol) == false)
                    {
                        CPositionManager::getInstance().UpdateLendingBalanceOnly(&update);
                    }
                    else if (update.side == ROMVALUE::SIDE_SHORT)
                    {
                        CPositionManager::getInstance().UpdateLendingBalanceOnly(&update);
                    }
                }
                else
                {
                    //Buy can not knock down the lending balance if the symbol is in Threshould list
                    if (CPositionManager::getInstance().FoundThresholdSymbol(update.symbol) == false)
                    {
                        update.qty_change *= -1;
                        CPositionManager::getInstance().UpdateLendingBalanceOnly(&update);
                    }
                }
            }
            free(rowE);
            destroy_queue(row);
        }

        destroy_queue(results);
    }
}


void CThreadRomOrdersNew::GetFuture(const std::string& product, std::deque<std::string>& futures)
{
    if (product.empty() != true)
    {
        double multiplier = 0;
        if (m_ProductMultiplier.Get(product, multiplier))
        {
            char buffer[128];
            sprintf(buffer, "%s=%f", product.c_str(), multiplier);
            futures.push_back(buffer);
        }
    }
    else
    {
        std::list<CRomMap<double>::tKeyValuePair> pairs;
        m_ProductMultiplier.GetKeyValuePairs(pairs);
        
        std::list<CRomMap<double>::tKeyValuePair>::iterator it = pairs.begin();
        char buffer[128];
        
        while (it != pairs.end())
        {
            sprintf(buffer, "%s=%f", (it->key).c_str(), it->value);
            futures.push_back(buffer);
            it++;
        }
    }
}

bool CThreadRomOrdersNew::ReloadFutures()
{
    return LoadProductMultiplierMap();
}

void CThreadRomOrdersNew::LoadPosUpdates(std::list<tPosUpdate*>& updates)
{
    std::list<CThreadOrderNew*> orders;
    m_RomTagToOrder.GetAll(orders);
    std::list<CThreadOrderNew*>::iterator it = orders.begin();
    CThreadOrderNew* pOrder = NULL;
    
    while (it != orders.end())
    {
        pOrder = *it;
        tPosUpdate* pUpdate = new tPosUpdate;
        if (pOrder->GetPosUpdate(*pUpdate))
        {
            updates.push_back(pUpdate);
        }
        else
        {
            delete pUpdate;
        }
        it++;
    }
}

void CThreadRomOrdersNew::MakeTheClientTag(std::string& theTag, const char* owner, const char* clienttag)
{
    theTag.append(owner, strlen(owner));
    theTag.append("-", 1);
    theTag.append(clienttag, strlen(clienttag));
}

void CThreadRomOrdersNew::UpdateBalanceFromLiveUserDefinedSpreadOrder(CRomOrderNew& order, std::vector<CRomOrderNew*>& legs)
{
    tKeyedBalanceUpdate update;
    memset(&(update.update), 0, sizeof(update.update));
    update.update.secIndex = order.SecIndex();
    update.clearing_sec = order.ClearingSecKey();
    update.clract = order.ClearingAccount();
    update.update.sideIndex = order.SideIndex();
    update.update.orderChange = 1;
    //update.username = order.Owner();
    update.username = order.Trader();

    long total_ratio = 0;
    bool foundEquty = false, foundOpt = false;
    
    for (size_t i = 0; i < legs.size(); i++)
    {
        if (legs[i]->SecType() == ROMVALUE::SECTYPE_EQUITY)
        {
            foundEquty = true;
        }
        else if (legs[i]->SecType() == ROMVALUE::SECTYPE_OPTION)
        {
            foundOpt = true;
        }
    }

    if (foundEquty && foundOpt)
    {
        order.CreateCoveredSpread(legs);
        update.update.shareChange = order.Leave();
        CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(update, const_cast<tCoveredSpread*>(order.CoveredSpread()), false);;
        /*for (size_t i = 0; i < legs.size(); i++)
        {
            ratio = legs[i]->Leave();
            if (ROMVALUE::SECTYPE_OPTION == legs[i]->SecType())
            {
                ratio *= 100;
            }
            total_ratio += ratio;
        }

        int roundUp = 0;
        if (total_ratio % 100)
        {
            roundUp = 1;
        }
        total_ratio = total_ratio / 100 + roundUp;*/
    }
    else
    {
        //what in leavesqty in DB is already TRUE leaves!
        //we decided to use originalshares in DB to store leg ratio!!!
        for (size_t i = 0; i < legs.size(); i++)
        {
            total_ratio += legs[i]->Leave();
        }

        update.update.shareChange = total_ratio;
        update.update.dollarChange = order.Multiplier() * order.Leave() * fabs(order.Price());
        order.SetRatio(total_ratio);
        CClientProfileManagerNew::getInstance().UpdateBalance(update);
    }
}

void CThreadRomOrdersNew::LoadSpreadOriginalSide(std::map<std::string, short>& spreadOriginalSide)
{
    std::string romInstance = CConfig::getInstance().InstanceName();
    char sqlstr[256];
    sprintf(sqlstr, "SELECT omtag, side FROM orders WHERE command='E' and cplxtype=%d and omname='%s'",
                            ROMVALUE::CPLXTYPE_SPREAD, romInstance.c_str());
    
    std::string sqlStr = sqlstr;
    
    queue_t results = create_queue();
    std::string errStr;
    queue_t row = NULL;
    element_t rowE; 
    
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        while (is_empty(results) == 0)
        {
            std::string omtag;
            short side;
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            while (is_empty(row) == 0)
            {
                element_t colE = pop_front(row);
                gendb_v* pVal = (gendb_v*)(colE->d);

                if (pVal->type_in_union != DB_NULL)
                {
                    switch(cnt)
                    {
                    case 0:
                        if (pVal->arr_len)
                        {
                            omtag.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 1:
                        side = (pVal->g_un).un_int;
                        spreadOriginalSide.insert(std::map<std::string, short>::value_type(omtag, side));
                        break;
                    default:
                        break;
                    }
                }

                cnt++;
                free(colE);
                gendb_destroy_v(pVal);
            }
            free(rowE);
            destroy_queue(row);
        } 
    }

    destroy_queue(results);
}