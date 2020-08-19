#include "ThreadRomOrders.h"
#ifdef _WIN32
#include "stdafx.h"
#endif

#include "RomCancel.h"
#include "RomReject.h"
#include "ClientProfileManager.h"
#include "DestsManager.h"
#include "romfielddefs.h"
#include "GStore.h"
//#include "generic_db.h"
#include "debug_logger.h"
#include "RomMapN.h"
#include <sstream>
#include <iomanip>

CThreadRomOrders::~CThreadRomOrders()
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

void CThreadRomOrders::ClearMaps()
{
    std::list<CThreadOrder*> orders;
    m_RomTagToOrder.GetAll(orders);
    std::list<CThreadOrder*>::iterator it = orders.begin();
    while (it != orders.end())
    {
        delete *it;
        it++;
    }

    if (m_FreeList.IsEmpty() == false)
    {
        std::deque<CThreadOrder*> freeOrders;
        m_FreeList.GetAll(freeOrders);
        std::deque<CThreadOrder*>::iterator it = freeOrders.begin();
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

void CThreadRomOrders::MapOrder(CThreadOrder* ptOrder, CRomOrder* pOrder)
{
    m_RomTagToOrder.Add(pOrder->RomTag(), ptOrder);

    if (strlen(pOrder->ParentRomTag()) == 0)
    {
        char value[200];
        unsigned int ownerLen = strlen(pOrder->Owner());
        unsigned int tagLen = strlen(pOrder->ClientTag());
        memcpy(value, pOrder->Owner(), ownerLen);
        memcpy(&(value[ownerLen]), pOrder->ClientTag(), tagLen);
        value[tagLen + ownerLen] = '\0';

        m_ClientTagToOrder.Add(value, ptOrder);

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

void CThreadRomOrders::StoreOrder(CRomOrder* pOrder)
{
    pthread_rwlock_wrlock(&m_lock);

    if (m_FreeList.IsEmpty())
    {
        //AllocateMemoryMap();
        ExpandFreeList();
    }

    CThreadOrder* ptOrder = NULL;
    m_FreeList.Get(ptOrder);
    ptOrder->SetOrder(pOrder);
    MapOrder(ptOrder, pOrder);

    /*
    CThreadOrder* ptOrder = NULL;
    if (m_FreeList.Get(ptOrder))
    {
        //pOrder->SetUsed(true);
        ptOrder->SetOrder(pOrder);
        MapOrder(ptOrder, pOrder);
    }
    else
    {
        printf("%s\n", "m_FreeList is empty()");
    }
    */

    pthread_rwlock_unlock(&m_lock);
}

CThreadRomOrders::CThreadRomOrders():
    m_RomTagToOrder(true),
    m_ClientTagToOrder(true),
    m_OwnerToOrderSet(true),
    m_DestToOrderSet(true),
    m_OrderNum(1, true)
{
    CRomOrder::Init();
    pthread_rwlock_init(&m_lock, NULL);
    SetDateInstanceStr();

    InitOrderNum();

    ExpandFreeList();

    if (CConfig::getInstance().QueryLiveOrderFromOats() == 0)
    {
        LoadOrdersFromDB();
    }
}

/*void CThreadRomOrders::DestroyOrder(CRomOrder* pOrder)
{
    pthread_rwlock_wrlock(&m_lock);

    destroyorder(pOrder);

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrders::destroyorder(CRomOrder* pOrder)
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

CRomOrder* CThreadRomOrders::RemoveOrder(const std::string& romTag)
{
    pthread_rwlock_wrlock(&m_lock);

    CRomOrder* pOrder = removeOrder(romTag);

    pthread_rwlock_unlock(&m_lock);

    return pOrder;
}

CRomOrder* CThreadRomOrders::removeOrder(const std::string& romTag)
{
    CThreadOrder* ptOrder = NULL;
    m_RomTagToOrder.Remove(romTag, ptOrder);
    CRomOrder* rc = NULL;

    //if (ptOrder != NULL &&
    //    strlen(pOrder->ParentRomTag()) == 0)
    if (ptOrder != NULL)
    {
        short cplxType = ptOrder->GetCplxType();
        if (cplxType != ROMVALUE::CPLXTYPE_LEG) //legs are only in romTag map!!!
        {
            CThreadSet<CThreadOrder*>* pOrders = NULL;
            char owner[128], dest[10], clientTag[128];
            ptOrder->GetOwner(owner, sizeof(owner));
            ptOrder->GetDest(dest, sizeof(dest));
            ptOrder->GetClientTag(clientTag, sizeof(clientTag));
            char value[200];
            sprintf(value, "%s%s", owner, clientTag);
            m_ClientTagToOrder.Remove(value, ptOrder);

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
bool CThreadRomOrders::AllocateMemoryMap()
{
    bool rc = false;
    unsigned long step = CConfig::getInstance().OrdersAllocStep();
    void* pOrders = calloc(step, sizeof(CRomOrder));

    if (pOrders)
    {
        m_OrderSets.insert(pOrders);
        CRomOrder* pOrder = (CRomOrder*)pOrders;

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

void CThreadRomOrders::ExpandFreeList()
{
    int step = CConfig::getInstance().OrdersAllocStep();
    CThreadOrder* ptOrder = NULL;

    for (int i = 0; i < step; i++)
    {
        ptOrder = new CThreadOrder();
        m_FreeList.Add(ptOrder);
    }
}

void CThreadRomOrders::BuildBalanceUpdateFromOrder(const CRomOrder& order, tBalanceUpdate& update)
{
    if (CRomOrder::IsLimitType(order.Type()))
    {
        update.dollarChange = order.SideSign() * order.Multiplier() * order.CurrencyRate() * order.Leave() * order.Price();
    }
    else
    {
        update.dollarChange = 0;
    }
    update.secType = order.SecType();
    update.orderChange = 1;
    update.shareChange = order.Leave();
}

bool CThreadRomOrders::GetOrderFromRomTag(const char* tag, CRomOrder* pOrder)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        ptOrder->GetOrder(pOrder);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrders::GetOrderFromClientTag(const std::string& tag, CRomOrder* pOrder)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_ClientTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        ptOrder->GetOrder(pOrder);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrders::GetOrderFromRomTagCancel(const char* tag,
                                                CRomOrder* pOrder,
                                                std::string& err, bool ownerCancel,
                                                CThreadOrderHandler& hd,
                                                CCsv* pCancel)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        rc = ptOrder->TurnOnCancelPending("", ownerCancel, pOrder, hd, pCancel, err);

        //ptOrder->GetOrder(pOrder);

        //if (pOrder->Leave() == 0)
        //{
        //    err = "There are not leaves to cancel";
        //    rc = false;
        //}
        //else
        //{
        //    if (ownerCancel)
        //    {
        //        ptOrder->SetCancelPending(true);
        //    }
        //}
        //ptOrder->GetNextRomExecTag(romExecTag);
    }
    else
    {
        err = "Order not found";
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrders::GetOrderFromClientTagCancel(const std::string& tag,
                                                   const std::string& actTag,
                                                   CRomOrder* pOrder,
                                                   std::string& err,
                                                   bool ownerCancel,
                                                   CThreadOrderHandler& hd,
                                                   CCsv* pCancel)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_ClientTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        rc =ptOrder->TurnOnCancelPending(actTag, ownerCancel, pOrder, hd, pCancel, err);
        //ptOrder->GetOrder(pOrder);

        //if (pOrder->Leave() == 0)
        //{
        //    err = "There are not leaves to cancel";
        //    rc = false;
        //}
       /*else
        {
            if (ownerCancel)
            {
                ptOrder->SetCancelPending(true);
            }
            ptOrder->SetActClientTag(actTag.c_str());
        }
        ptOrder->GetNextRomExecTag(romExecTag);*/
    }
    else
    {
        err = "Order not found";
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadRomOrders::GetOrderFromClientTagReplace(const std::string& tag, CCsv& msg, CRomOrder* pOrder, std::string& err)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_ClientTagToOrder.Get(tag, ptOrder);
    if (rc)
    {
        //ptOrder->GetOrder(pOrder);
        if (ptOrder->ReplacePending() == true)
        {
            err = "Previous Replace is still pending in ROM";
            ptOrder->GetOrder(pOrder);
            rc = false;
        }
        else
        {
            ptOrder->UpdateReplacingFields(msg);
            ptOrder->GetOrder(pOrder);

            if (pOrder->ShareChange() + pOrder->Leave() <= 0)
            {
                ptOrder->ResetReplacingFields();
                char text[100];
                sprintf(text, "New Order Share has to be larger than current Cum(%ld)", pOrder->Cum());
                err = text;
                rc = false;
            }
            else
            {
                ptOrder->SetReplacePending(true);
            }

            std::string romExecTag;
            ptOrder->GetNextRomExecTag(romExecTag);
            msg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExecTag);
        }
    }
    else
    {
        err = "Order not found";
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}


bool CThreadRomOrders::GetNextRomExecTag(const std::string& romTag, std::string& romExecTag)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;

    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->GetNextRomExecTag(romExecTag);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

//void CThreadRomOrders::BuildCancels(CCsv& filter, std::deque<COatsMsg*>& oatsMsgs, std::set<CCsv*>& cancels, bool dayorderonly)
void CThreadRomOrders::BuildCancels(CCsv& filter, std::set<CCsv*>& cancels, bool dayorderonly)
{
    pthread_rwlock_rdlock(&m_lock);

    std::string dest = "-1";
    filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
    std::string owner;
    tOrderSet* ptOrderSet = NULL;
    std::string omTime;
    if (filter.GetAt(ROMFIELDS::CSV_OM_TIME, omTime) == false)
    {
        char t[22];
        CUtility::GetGmtTimeStr(t, sizeof(t));
        omTime = t;
    }
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

    std::set<CThreadOrder*> orderSet;
    ptOrderSet->GetAll(orderSet);
    std::set<CThreadOrder*>::iterator it = orderSet.begin();
    CThreadOrder* ptOrder = NULL;
    CRomOrder order;
    std::string err = "";

    while (it != orderSet.end())
    {
        ptOrder = *it;
        ptOrder->GetOrder(&order);

        if (order.DeadSpread() ||
            (dayorderonly &&
             (order.TIF() == ROMVALUE::TIF_GTC || order.TIF() == ROMVALUE::TIF_GTD)))
        {
            it++;
            continue;
        }

        if (Filter(order, filter) == false)
        {
            //CRomCancel* pCancel = new CRomCancel(order);
            CCsv* pCancel = CCsvStore::getInstance().Get();
            pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
            pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, order.RomTag());
            pCancel->SetAt(ROMFIELDS::CSV_TAG, order.ClientTag());
            pCancel->SetAt(ROMFIELDS::CSV_OWNER, order.Owner());

            //order.GetCsv(*pCancel);

            //if (ownerCancel)
            //{
            //    ptOrder->SetCancelPending(true);
            //}

            if (strcmp(order.Owner(), owner.c_str()) == 0)
            {
                short ownercancel = 0;
                filter.GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
                pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
            }

            //CDestsManager::getInstance().Send(order.Dest(), cancel, err);

            //pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            //pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
            //std::string newtag;
            //pCancel->GetAt(ROMFIELDS::CSV_NEW_TAG, newtag);
            //order.SetActClientTag(newtag.c_str());
            //std::string omExTag;
            //GetNextRomExecTag(order.RomTag(), omExTag);
            //pCancel->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
            pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

            //COatsMsg* pMsg = new COatsMsg(pCancel, &order, true, true);
            //oatsMsgs.push_back(pMsg);

            //pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
            //pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
            //pCancel->SetAt(ROMFIELDS::CSV_ORDERDEST, order.Dest());
            //cancels.push_back(pCancel);
            cancels.insert(pCancel);
            //CDestsManager::getInstance().Send(order.Dest(), cancel, err);
        }
        it++;
    }

    pthread_rwlock_unlock(&m_lock);
}

//void CThreadRomOrders::BuildDestCancels(const std::string& destID, std::deque<COatsMsg*>& oatsMsgs, std::set<CCsv*>& cancels)
void CThreadRomOrders::BuildDestCancels(const std::string& destID, std::set<CCsv*>& cancels)
{
    pthread_rwlock_rdlock(&m_lock);

    tOrderSet* ptOrderSet = NULL;
    if (m_DestToOrderSet.Get(destID, ptOrderSet))
    {
        std::set<CThreadOrder*> orderSet;
        ptOrderSet->GetAll(orderSet);
        std::set<CThreadOrder*>::iterator it = orderSet.begin();
        CThreadOrder* ptOrder = NULL;
        CRomOrder order;

        char omTime[22];
        CUtility::GetGmtTimeStr(omTime, sizeof(omTime));

        while (it != orderSet.end())
        {
            ptOrder = *it;
            ptOrder->GetOrder(&order);

            if (order.TIF() != ROMVALUE::TIF_GTD &&
                order.TIF() != ROMVALUE::TIF_GTC &&
                order.DeadSpread() == false)
            {
                CCsv* pCancel = CCsvStore::getInstance().Get();
                pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
                pCancel->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, order.RomTag());
                pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 0);
                //order.GetCsv(*pCancel);

                pCancel->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
                //pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
                //pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
                //std::string newtag;
                //pCancel->GetAt(ROMFIELDS::CSV_NEW_TAG, newtag);
                //order.SetActClientTag(newtag.c_str());
                //std::string omExTag;
                //GetNextRomExecTag(order.RomTag(), omExTag);
                //pCancel->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
                //COatsMsg* pMsg = new COatsMsg(pCancel, &order, true);
                //oatsMsgs.push_back(pMsg);

                //pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
                //pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
                //pCancel->SetAt(ROMFIELDS::CSV_ORDERDEST, order.Dest());
                cancels.insert(pCancel);
            }

            it++;
        }
    }
    pthread_rwlock_unlock(&m_lock);
}

//void CThreadRomOrders::BuildAllCancels(std::deque<COatsMsg*>& oatsMsgs, std::set<CRomCancel*>& cancels)
//{
//}

/*
void CThreadRomOrders::Cancels(CCsv& filter, std::list<COatsMsg*>& oatsMsgs)
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
  CRomOrder order;
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

bool CThreadRomOrders::Filter(const CRomOrder& order, CCsv& filter)
{
    char secType;

    if (filter.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType) == true )//filter.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE) == false)
    {
        if ((CRomOrder::IsEquityType(secType) == true &&
                CRomOrder::IsEquityType(order.SecType()) != true) ||
                (CRomOrder::IsOptionType(secType) == true &&
                 CRomOrder::IsOptionType(order.SecType()) != true) ||
                (CRomOrder::IsFutureType(secType) == true &&
                 CRomOrder::IsFutureType(order.SecType()) != true) ||
                (CRomOrder::IsCurrencyType(secType) == true &&
                 CRomOrder::IsCurrencyType(order.SecType()) != true)) // ||
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

    if (CRomOrder::IsOptionType(order.SecType()))
    {
        std::string underlying;
        if (filter.GetAt(ROMFIELDS::CSV_UNDERLYING, underlying) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING) == false &&
                strcmp(order.Underlying(), underlying.c_str()) != 0)
        {
            return true;
        }

        double strike = 0, lstrike = 0;
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
        }

        char callput;
        if (filter.GetAt(ROMFIELDS::CSV_CALLPUT, callput) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_CALLPUT) == false &&
                order.CallOrPut() != callput)
        {
            return true;
        }
    }

    if (CRomOrder::IsEquityType(order.SecType()) == false)
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

void CThreadRomOrders::CleanUp()
{
    ClearMaps();
}

void CThreadRomOrders::InitOrderNum()
{
    if (CConfig::getInstance().IsFirstRun() == false)
    {
        time_t ticks = time(NULL);
        struct tm *tmStruct = gmtime(&ticks);
        unsigned long long seconds = 3600 * tmStruct->tm_hour + 60 * tmStruct->tm_min + tmStruct->tm_sec + 1;
        m_OrderNum.Reset(1000000 * seconds);
    }
}

bool CThreadRomOrders::SetDateInstanceStr()
{
    bool rc = false;
    time_t t;
    time(&t);
    struct tm tmNow, tmCentralTime;
    localtime_r(&t, &tmNow);
    CUtility::AdjustToCentralTime(tmNow, tmCentralTime);

    std::stringstream stm;
    stm << CConfig::getInstance().InstanceName().c_str()
        << std::setw(4) << 1900 + tmCentralTime.tm_year
        << std::setw(2) << std::setfill('0') << tmCentralTime.tm_mon+1
        << std::setw(2) << std::setfill('0') << tmCentralTime.tm_mday ;

    if (m_DateInstanceStr.compare(stm.str()) != 0)
    {
        m_DateInstanceStr = stm.str();
        rc = true;
    }

    return rc;
}

void CThreadRomOrders::GetNextRomTag(const std::string& owner, std::string& romTag)
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

    unsigned long long num = m_OrderNum.Increment();
    char value[200];
    sprintf(value, "%s-%s-%llu", owner.c_str(), CConfig::getInstance().SessionDateInstanceStr().c_str(),  num); //%lu???

    romTag = value;
}

void CThreadRomOrders::ResetReplacingFields(const std::string& romTag)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;

    if (m_RomTagToOrder.Get(romTag, ptOrder))
    {
        ptOrder->ResetReplacingFields();
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrders::UpdateClientTag(const char* oldtag, const char* newtag)
{
    if (strcmp(newtag, "") == 0 ||
            strcmp(oldtag, newtag) == 0)
    {
        return;
    }

    pthread_rwlock_wrlock(&m_lock);

    CThreadOrder* ptOrder = NULL;

    if (m_ClientTagToOrder.Remove(oldtag, ptOrder))
    {
        m_ClientTagToOrder.Add(newtag, ptOrder);
    }

    pthread_rwlock_unlock(&m_lock);
}

bool CThreadRomOrders::UpdateOrder(const std::string& romTag, CCsv& msg, CRomOrder* pOrder, std::string& err)
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
            ptOrder->UpdateOrder(msg, pOrder);
        }
        else
        {
            std::string romExTag;
            ptOrder->GetOrderWithRomExTag(pOrder, romExTag);
            msg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
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

    /*char cmd = 'S';
    if (msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd) &&
        cmd == ROMVALUE::COMMAND_ZAP)
    {
        if (pOrder->Status() == ROMVALUE::STATUS_NEW)
        {
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
        }
        else
        {
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, "Zapped");
        msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        pOrder->SetLeave(0);
        pOrder->SetReplacePending(false);
    }*/

    if (pOrder->Leave() <= 0 &&
        pOrder->Status() != ROMVALUE::STATUS_REPLACED &&
        pOrder->ReplacePending() == false &&
        pOrder->CplxType() != ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
    {
        //DestroyOrder(pOrder);
        CRomOrder* pRomOrder = RemoveOrder(romTag);
        if (pRomOrder)
        {
            CGStore::getInstance().Return(pRomOrder);
        }
    }

    return rc;
}

bool CThreadRomOrders::LoadOrdersFromDB()
{
    bool rc = true;
    queue_t results = create_queue();
    const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    int romInstance = CConfig::getInstance().Instance();
    char sqlStr[200];
    sprintf(sqlStr, "%s.dbo.ROM_selOrdersNew %d", db.database.c_str(), romInstance);

    //send_debug_message("Starting querying live order from DB\n");
    printf("Starting querying live order from DB\n");

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    //if (CUtility::DbExec(sqlStr, db.dbserver.c_str(), db.user.c_str(), db.password.c_str(), &results))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        CRomMap<CRomOrder*> parents;
        CRomMap<long> RomTagToRatio;
        std::set<CRomOrder*> children;
        std::set<std::string> parentUpdatCandidatorSelected;

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            //should get from GStore<CRomOrder*>
            CRomOrder* pOrder = CGStore::getInstance().Get();

            bool skip = false;
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
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetDBSymbol(strvalue.c_str());
                        }
                        break;
                    case 9:
                    {
                        char destStr[4];
                        sprintf(destStr, "%d", (pVal->g_un).un_int);
                        pOrder->SetExchange(destStr);
                    }
                    break;
                    case 10:
                    {
                        char destStr[4];
                        sprintf(destStr, "%d", (pVal->g_un).un_int);
                        pOrder->SetDest(destStr);
                    }
                    break;
                    case 11:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetExDest(strvalue.c_str());
                        }
                        break;
                    case 12:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetExpYearMonth(strvalue.c_str());
                        }
                        break;
                    case 13:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetFirm(strvalue.c_str());
                        }
                        break;
                    case 14:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetGiveUpAccount(strvalue.c_str());
                        }
                        break;
                    case 15:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetInstructions(strvalue.c_str());
                        }
                        break;
                    case 16:
                        pOrder->SetLeave((pVal->g_un).un_int);
                        pOrder->SetShares(pOrder->Cum() + pOrder->Leave());
                        break;
                    case 17:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetLocalAccount(strvalue.c_str());
                        }
                        break;
                    case 18:
                    {
                        char maxFloor[10];
                        sprintf(maxFloor, "%d", (pVal->g_un).un_int);
                        pOrder->SetMaxFloor(maxFloor);
                    }
                    break;
                    case 19:
                        pOrder->SetMinQty((pVal->g_un).un_int);
                        break;
                    case 20:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetMMAccount(strvalue.c_str());
                        }
                        break;
                    case 21:
                        pOrder->SetMMExchange((pVal->g_un).un_int);
                        break;
                    case 22:
                        pOrder->SetMultiplier((pVal->g_un).un_double);
                        break;
                    case 23:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            //std::string romTag = (pVal->g_un).un_array;

                            /*int romInstance = CUtility::GetRomIDFromRomTag(romTag);

                            if (romInstance != CConfig::getInstance().Instance())
                            {
                                skip = true; // Do not pick up the orders belonging to other ROM instances!!
                            }
                            else
                            {
                                order.SetRomTag(romTag.c_str());
                            }*/

                            pOrder->SetRomTag(strvalue.c_str());
                        }
                        else
                        {
                            skip = true;
                        }
                        break;
                    case 24:
                        pOrder->SetOpenClose((pVal->g_un).un_int);
                        break;
                    case 25:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetExpTime(strvalue.c_str());
                        }
                        break;
                    case 26:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetRomExchangeTag(strvalue.c_str());
                        }
                        break;
                    case 27:
                        pOrder->SetType((pVal->g_un).un_int);
                        break;
                    case 28:
                        pOrder->SetOriginalShares((pVal->g_un).un_int);
                        break;
                    case 29:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetOwner(strvalue.c_str());
                        }
                        break;
                    case 30:
                        pOrder->SetPrice((pVal->g_un).un_double);
                        break;
                    case 31:
                        if (pVal->arr_len)
                        {
                            pOrder->SetSecType((pVal->g_un).un_array[0]);
                        }
                        break;
                    case 32:
                        pOrder->SetSide((pVal->g_un).un_int);
                        break;
                    case 33:
                        pOrder->SetStatus((pVal->g_un).un_int);
                        //edge case
                        if (pOrder->Status() == ROMVALUE::STATUS_ROMREPLACEPENDING)
                        {
                            pOrder->SetReplacePending(true);
                            pOrder->SetLoadedReplacePending(true);
                        }
                        else if (pOrder->Status() == ROMVALUE::STATUS_ROMCANCELPENDING)
                        {
                            pOrder->SetCancelPending(true);
                        }
                        break;
                    case 34:
                        {
                            char tmp[100];
                            sprintf(tmp, "%f", (pVal->g_un).un_double);
                            pOrder->SetStopPrice(tmp);
                        }
                        break;
                    case 35:
                        pOrder->SetStrikePrice((pVal->g_un).un_double);
                        break;
                    case 36:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetSymbol(strvalue.c_str());
                        }
                        break;
                    case 37:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetClientTag(strvalue.c_str());
                        }
                        break;
                    case 38:
                        pOrder->SetTIF((pVal->g_un).un_int);
                        if (pOrder->TIF() != ROMVALUE::TIF_GTC)
                        {
                            //if (CUtility::IsTheSessionOrder(CConfig::getInstance().SessionDateStr(),
                            //                                pOrder->RomTag()) == false)
                            //{
                            //    skip = true;
                            //}
                        }
                        break;
                    case 39:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetTradeFor(strvalue.c_str());
                        }
                        break;
                    case 40:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetTrader(strvalue.c_str());
                        }
                        break;
                    case 41:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetUnderlying(strvalue.c_str());
                        }
                        break;
                    case 42:
                        if (pVal->arr_len)
                        {
                            pOrder->SetExecInst((pVal->g_un).un_array[0]);
                        }
                        break;
                    case 43:
                        /*if (strcmp((pVal->g_un).un_array, "") != 0)
                        {
                            pOrder->SetNextExecIdFromExecTag((pVal->g_un).un_array);
                        }
                        else
                        {
                            pOrder->SetNextExecId(0);
                        }*/
                        break;
                    case 44:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetParentRomTag(strvalue.c_str());
                        }
                        break;
                    case 45:
                        if (pVal->arr_len)
                        {
                            strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                            pOrder->SetSecID(strvalue.c_str());
                        }
                        break;
                    case 46:
                        pOrder->SetCplxType((pVal->g_un).un_int);
                        break;
                    case 47:
                        if (pVal->type_in_union != DB_NULL)
                        {
                            char pegoffset[10];
                            sprintf(pegoffset, "%f", (pVal->g_un).un_double);
                            pOrder->SetPegOffset(pegoffset);
                        }
                        break;
                    case 48:
                        if (pVal->type_in_union != DB_NULL)
                        {
                            if (pVal->arr_len)
                            {
                                strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                                pOrder->SetSecDef(strvalue.c_str());
                            }
                        }
                        break;
                    case 49:
                        if (pVal->type_in_union != DB_NULL)
                        {
                            if (CRomOrder::IsOptionType(pOrder->SecType()))
                            {
                                if (pVal->arr_len == 8)
                                {
                                    strvalue.assign(&((pVal->g_un).un_array[6]), 2);
                                    pOrder->SetMaturityDay(strvalue.c_str());
                                }
                            }
                            else
                            {
                                if (pVal->arr_len >= 6)
                                {
                                    strvalue.assign((pVal->g_un).un_array, pVal->arr_len);
                                    pOrder->SetExpYearMonth(strvalue.substr(0, 6).c_str());
                                }
                            }
                        }
                        break;
                    case 50:
                        if (pVal->type_in_union != DB_NULL)
                        {
                            pOrder->SetAlgoType((pVal->g_un).un_int);
                        }
                        break;
                    case 51:
                        pOrder->SetNextExecId((pVal->g_un).un_int + 1);
                        break;
                    case 52:
                        {
                            if (pOrder->TIF() != ROMVALUE::TIF_GTC)
                            {
                                std::string timeStamp;
                                if (pVal->type_in_union != DB_NULL)
                                {
                                    timeStamp.assign((pVal->g_un).un_array, pVal->arr_len);
                                    tSC sc;
                                    if (CDestsManager::getInstance().GetDestSC(pOrder->Dest(), sc))
                                    {
                                        skip = !CUtility::IsTheSessionOrder(sc.openTime, sc.closeTime, sc.resetWDay, timeStamp.c_str());
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }

                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
                cnt++;
            }

            if (true == skip)
            {
                CGStore::getInstance().Return(pOrder);
            }
            else
            {
                pOrder->MakeClearinSecKey();

                if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SPREAD)
                {
                    parents.Add(pOrder->RomTag(), pOrder);
                }

                if (strlen(pOrder->ParentRomTag()) != 0)
                {
                    pOrder->SetShares(pOrder->OriginalShares());

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
                }
            }

            free(rowE);
            destroy_queue(row);
        }

        std::list<CRomMap<long>::tKeyValuePair> keyvalues;
        RomTagToRatio.GetKeyValuePairs(keyvalues);
        std::list<CRomMap<long>::tKeyValuePair>::iterator it = keyvalues.begin();
        CRomOrder* pParent = NULL;

        while (it != keyvalues.end())
        {
            if (parents.Get(it->key, pParent) == true) // &&
                //pParent->SecType() != ROMVALUE::SECTYPE_COMPLEX)
            {
                pParent->SetRatio(it->value);
                UpdateBalanceFromLiveOrder(*pParent);
            }

            it++;
        }

        CRomOrder* pChild = NULL;
        std::set<CRomOrder*>::iterator itt = children.begin();
        while (itt != children.end())
        {
            //It is not guaranteed that exchange will send ROM reject for all the legs
            //DB or OATS may have legs live while body has already gone!!
            pChild = *itt;
            if (parents.Find(pChild->ParentRomTag()) == true)
            {
                StoreOrder(pChild);
            }

            itt++;
        }

        //send_debug_message("Finished querying live order from DB\n");
        printf("Finished querying live order from DB\n");
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

    return rc;
}


bool CThreadRomOrders::FillParent(const std::string& parentRomTag, long lastShares, tBalanceUpdate& BUpdate, CRomOrder* pParent)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(parentRomTag, ptOrder);
    if (rc)
    {
        ptOrder->UpdateFromChildFill(lastShares, BUpdate, pParent);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CThreadRomOrders::UpdateLegsReplace(const std::string& owner, const std::vector<CCsv*>& legs, long shareChange)
{
    CCsv* pCsv = NULL;
    CThreadOrder* ptOrder = NULL;
    std::string clienttag = owner;
    std::string tag;

    pthread_rwlock_rdlock(&m_lock);

    for (size_t i = 0; i < legs.size(); i++)
    {
        pCsv = legs[i];

        if (pCsv->GetAt(ROMFIELDS::CSV_TAG, tag))
        {
            clienttag.append(tag);
            if (m_ClientTagToOrder.Get(clienttag, ptOrder))
            {
                ptOrder->SetShareChange(shareChange, tag);
                pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, tag);
            }
        }
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrders::StoreLiveOrders(const std::set<databuf_t*>& bufs)
{
    std::set<databuf_t*>::iterator it = bufs.begin();
    databuf_t* pBuf = NULL;
    CRomMap<CRomOrder*> parents;
    CRomMap<long> RomTagToRatio;
    std::set<std::string> parentUpdatCandidatorSelected;
    std::set<CRomOrder*> children;

    while (it != bufs.end())
    {
        pBuf = *it;
        CCsv* pCsv = CCsvStore::getInstance().Get();
        pCsv->Set(pBuf->buffer, databuf_unread(pBuf));
        CRomOrder* pOrder = CGStore::getInstance().Get();
        pOrder->Set(*pCsv);
        //restore CancelPending/ReplacePending
        if (pOrder->Status() == ROMVALUE::STATUS_ROMREPLACEPENDING )
        {
            pOrder->SetReplacePending(true);
        }
        else if (pOrder->Status() == ROMVALUE::STATUS_ROMCANCELPENDING)
        {
            pOrder->SetCancelPending(true);
        }

        CCsvStore::getInstance().Return(pCsv);
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
        }
        it++;
    }

    std::list<CRomMap<long>::tKeyValuePair> keyvalues;
    RomTagToRatio.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<long>::tKeyValuePair>::iterator itt = keyvalues.begin();
    CRomOrder* pParent = NULL;

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

    CRomOrder* pChild = NULL;
    std::set<CRomOrder*>::iterator ittt = children.begin();
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
}

void CThreadRomOrders::UpdateBalanceFromLiveOrder(const CRomOrder& order)
{
    tBalanceUpdate update;
    update.secType = order.SecType();
    update.orderChange = 1;
    update.shareChange = order.Leave() * order.Ratio();

    if (CRomOrder::IsLimitType(order.Type()))
    {
        update.dollarChange = order.SideSign() * order.Multiplier() * order.CurrencyRate() * order.Leave() * order.Price();
    }
    else
    {
        update.dollarChange = 0;
    }

    CClientProfileManager::getInstance().UpdateBalance(order.Trader(), update);
    std::string key = order.ClearingSecKey();
    CClientProfileManager::getInstance().UpdateClearingActSecBalance(key, update);
}

/*
void CThreadRomOrders::UpdateBalanceFromLiveOrder(const CRomOrder& order)
{
    //regarding user defined spread to make things correct the following need to be done!!!
    // 1) contribution from order with different leg security type should be skipped since it was
    //    never included when the order was created
    // 2) leg candidate that is responsible for updating body balance should be selected
    // The above have not been implemented in version 2.9.0!!!

    if (order.CplxType() == ROMVALUE::CPLXTYPE_SINGLE ||
        order.CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
    {
        tBalanceUpdate update;
        BuildBalanceUpdateFromOrder(order, update);
        CClientProfileManager::getInstance().UpdateBalance(order.Trader(), update);
        std::string key = order.ClearingSecKey();
        CClientProfileManager::getInstance().UpdateClearingActSecBalance(key, update);
    }
    else if (order.CplxType() == ROMVALUE::CPLXTYPE_SPREAD)
    {
        tBalanceUpdate update;
        update.dollarChange = order.Leave() * order.Price() * order.Multiplier();
        update.orderChange = 1;
        update.shareChange = 0;
        update.secType = order.SecType();
        CClientProfileManager::getInstance().UpdateBalance(order.Trader(), update);
        std::string key = order.ClearingSecKey();
        CClientProfileManager::getInstance().UpdateClearingActSecBalance(key, update);
    }
    else if (strlen(order.ParentRomTag()) != 0)
    {
        tBalanceUpdate update;
        update.dollarChange = 0;
        update.orderChange = 0;
        update.shareChange = order.Leave();
        update.secType = order.SecType();
        CClientProfileManager::getInstance().UpdateBalance(order.Trader(), update);
        std::string key = order.ClearingSecKey();
        CClientProfileManager::getInstance().UpdateClearingActSecBalance(key, update);
    }
}*/

void CThreadRomOrders::SetRatio(const std::string& romTag, long value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetRatio(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrders::SetCancelPending(const std::string& romTag, bool value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetCancelPending(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}

void CThreadRomOrders::SetReplacePending(const std::string& romTag, bool value)
{
    pthread_rwlock_rdlock(&m_lock);

    CThreadOrder* ptOrder = NULL;
    bool rc = m_RomTagToOrder.Get(romTag, ptOrder);
    if (rc)
    {
        ptOrder->SetReplacePending(value);;
    }

    pthread_rwlock_unlock(&m_lock);
}
