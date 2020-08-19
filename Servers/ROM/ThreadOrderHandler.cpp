#include "ThreadOrderHandler.h"
#include "ClientsManager.h"
#include "DestsManager.h"
#include "RomReject.h"
#include "RomOrder.h"
#include "BuffStore.h"
#include "GStore.h"

typedef struct
{
    CRomOrder* pOrder;
    CCsv* pCsv;
} tKid;  //for reject purpose!!!

CThreadOrderHandler::CThreadOrderHandler() : m_ReplacingDelts(true)
{
    m_pOrders = new CThreadRomOrders();
}

CThreadOrderHandler::~CThreadOrderHandler()
{
    if (m_pOrders)
    {
        delete m_pOrders;
    }
}

bool CThreadOrderHandler::ValidateOrder(const CRomOrder& order, const std::vector<CCsv*>* pLegs, tBalanceUpdate& BUpdate, std::string& error)
{
    //make sure order tag not duplicated
    CRomOrder dummy;
    std::string romClientTag = order.Owner();
    romClientTag.append(order.ClientTag());
    if (m_pOrders->GetOrderFromClientTag(romClientTag, &dummy))
    {
        error = "Duplicated order tag:" + romClientTag;
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

    bool rc = false;
    if (order.CplxType() == ROMVALUE::CPLXTYPE_SPREAD)
    {
        rc = CClientProfileManager::getInstance().AllowedToPlaceOrder_UserDefinedSpread(order, pLegs, BUpdate, error);
    }
    else
    {
        rc = CClientProfileManager::getInstance().AllowedToPlaceOrder(order, BUpdate, error);
    }

    return rc;
}

bool CThreadOrderHandler::ProcessSingleOrder(CCsv* pCsv)
{
    std::string err = "";
    COats::tMsgList oatsMsgs;
    std::string owner, omTime;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    pCsv->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    std::string romtag;
    m_pOrders->GetNextRomTag(owner, romtag);
    pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
    std::string routeID, destID;
    pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
    CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
    pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

    //CRomOrder order(*pCsv);
    CRomOrder* pOrder = CGStore::getInstance().Get();
    pOrder->SingleSet(*pCsv);

    if (CRomOrder::IsCurrencyType(pOrder->SecType()) == true)
    {
        std::string symbol = pOrder->Symbol();
        pOrder->SetCurrency((symbol.substr(0, 3)).c_str());
    }
    else
    {
        pOrder->SetCurrency("USD");
    }

    pOrder->SetCurrencyRate(1);
    pOrder->SetDest(routeID.c_str());

    std::string omExTag;
    COatsMsg* pOatsMsg = NULL;

    tBalanceUpdate theChange = {0,0,0,ROMVALUE::SECTYPE_EQUITY};

    if (!ValidateOrder(*pOrder, NULL, theChange, err))
    {
        pOrder->NextRomExecTag(omExTag);
        pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        pOatsMsg = new COatsMsg(pCsv, pOrder, true);
        DoOats(pOatsMsg);

        MakeOrderReject(*pCsv, err);
        CClientsManager::getInstance().Send(owner, *pCsv, err);

        pOrder->NextRomExecTag(omExTag);
        pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        pOatsMsg = new COatsMsg(pCsv, pOrder, false);
        DoOats(pOatsMsg);
    }
    else
    {
        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        pCsv->GetMsgNew(pBuffer);

        pOrder->NextRomExecTag(omExTag);
        pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        pOatsMsg = new COatsMsg(pCsv, pOrder, false);
        DoOats(pOatsMsg);
        m_pOrders->StoreOrder(pOrder);

        if (CDestsManager::getInstance().Send(routeID, ROMVALUE::COMMAND_ORDER, pBuffer->buffer, databuf_unread(pBuffer), err) == false)
        {
            CCsv* pReject = CCsvStore::getInstance().Get();
            MakeOrderReject(*pReject, *pOrder, err);
            pReject->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

            m_pOrders->GetNextRomExecTag(pOrder->RomTag(), omExTag);

            //pOrder->NextRomExecTag(omExTag);
            pReject->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
            CClientsManager::getInstance().Send(owner, *pReject, err);
            pOatsMsg = new COatsMsg(pReject, pOrder, false);
            DoOats(pOatsMsg);

            //back out the ballance contribution
            theChange.orderChange *= -1;
            theChange.shareChange *= -1;
            theChange.dollarChange *= -1;

            CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), theChange);
            CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), theChange);
            //m_pOrders->DestroyOrder(&order);
            m_pOrders->RemoveOrder(pOrder->RomTag());
            CGStore::getInstance().Return(pOrder);
        }

        CBuffStore::getInstance().Return(pBuffer);
    }

    return false;
}

bool CThreadOrderHandler::ProcessUserDefinedSpreadOrder(CCsv* pBody, std::vector<CCsv*>* pLegs)
{
    bool sent = false;

    std::string owner, error, romtag, omTime;
    CRomOrder* pOrder = NULL;

    pBody->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    pBody->GetAt(ROMFIELDS::CSV_OWNER, owner);
    m_pOrders->GetNextRomTag(owner, romtag);
    pBody->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

    std::string routeID, destID;
    pBody->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
    CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
    pBody->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

    pOrder = CGStore::getInstance().Get();
    pOrder->SingleSet(*pBody);

    if (CRomOrder::IsCurrencyType(pOrder->SecType()) == true)
    {
        std::string symbol = pOrder->Symbol();
        pOrder->SetCurrency((symbol.substr(0, 3)).c_str());
    }
    else
    {
        pOrder->SetCurrency("USD");
    }

    pOrder->SetCurrencyRate(1);
    pOrder->SetDest(routeID.c_str());

    COats::tMsgList oatsMsgs;
    std::string omExTag;
    COatsMsg* pMsg = NULL;
    //pOrder->NextRomExecTag(omExTag);
    //pBody->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
    //COatsMsg* pMsg = new COatsMsg(pBody, pOrder, false);
    //oatsMsgs.push_back(pMsg);


    std::vector<tKid> childList;
    CCsv* pM = NULL;
    //char preSecType = '0', curSecType = '0';

    bool foundOpt = false, foundEqt = false;

    for (size_t i = 0; i < pLegs->size(); i++)
    {
        pM = (*pLegs)[i];
        pM->SetAt(ROMFIELDS::CSV_OWNER, pOrder->Owner());
        pM->SetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, pOrder->RomTag());
        pM->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, ROMVALUE::CPLXTYPE_LEG);

        m_pOrders->GetNextRomTag(owner, romtag);
        pM->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

        CRomOrder* pChild = CGStore::getInstance().Get();
        pChild->SingleSet(*pM);
        //curSecType = pChild->SecType();
        //if (0 == i)
        //{
        //    preSecType = curSecType;
        //}

        //if (preSecType != curSecType)
        //{
        //    pOrder->SetSecType(ROMVALUE::SECTYPE_COMPLEX);
        //}
        if (pChild->SecType() == ROMVALUE::SECTYPE_OPTION)
        {
            foundOpt = true;
        }
        else if (pChild->SecType() == ROMVALUE::SECTYPE_EQUITY)
        {
            foundEqt = true;
        }

        pChild->SetRatio(pChild->Shares());

        pChild->SetRomTag(romtag.c_str());
        pChild->InheritParent(pOrder);

        pChild->NextRomExecTag(omExTag);
        pM->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

        if (CConfig::getInstance().mkOats() == 0)
        {
            //otherwise database will not have inherited fields
            CCsv* pCsv = CCsvStore::getInstance().Get();
            pChild->GetCsv(*pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
            pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            pMsg = new COatsMsg(pCsv, pChild, false);
        }
        else
        {
            pM->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            pMsg = new COatsMsg(pM, pChild, true);
        }
        //pMsg = new COatsMsg(pM, pChild, true);
        oatsMsgs.push_back(pMsg);

        tKid kid = {pChild, pM};
        childList.push_back(kid);
    }

    if (foundOpt == true && foundEqt == true)
    {
        pOrder->SetMixedSpread(true);
    }

    pOrder->NextRomExecTag(omExTag);
    //pOrder->SetSecType(curSecType);
    pBody->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
    //pMsg = new COatsMsg(pBody, pOrder, false);
    //oatsMsgs.push_front(pMsg);

    tBalanceUpdate BUpdate = {0,0,0, ROMVALUE::SECTYPE_EQUITY};
    databuf_t* pBuffer = NULL;

    if (!ValidateOrder(*pOrder, pLegs, BUpdate, error))
    {
        pMsg = new COatsMsg(pBody, pOrder, true);
        oatsMsgs.push_front(pMsg);

        MakeOrderReject(*pBody, error);
        pBuffer = CBuffStore::getInstance().Get();
        pBody->GetMsgNew(pBuffer);
        CClientsManager::getInstance().Send(owner, pBuffer->buffer, databuf_unread(pBuffer), error);
        CBuffStore::getInstance().Return(pBuffer);

        //pOrder->NextRomExecTag(omExTag);
        //pBody->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        //COatsMsg* pMsg = new COatsMsg(pBody, pOrder, true, true);

        //pBody->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);
        //pBody->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
        //pBody->SetAt(ROMFIELDS::CSV_TEXT, "");

        //oatsMsgs.push_back(pMsg);
        //DoOats(oatsMsgs);

        for (size_t i = 0; i < pLegs->size(); i++)
        {
            MakeOrderReject(*(childList[i].pCsv), error);
            childList[i].pOrder->NextRomExecTag(omExTag);
            childList[i].pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
            pMsg = new COatsMsg(childList[i].pCsv, childList[i].pOrder, true, true);
            oatsMsgs.push_back(pMsg);
        }

        pOrder->NextRomExecTag(omExTag);
        pBody->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        //COatsMsg* pMsg = new COatsMsg(pBody, pOrder, true, true);
        pMsg = new COatsMsg(pBody, pOrder, false);
        oatsMsgs.push_back(pMsg);

        DoOats(oatsMsgs);
        sent = false;
    }
    else
    {
        pMsg = new COatsMsg(pBody, pOrder, false);
        oatsMsgs.push_front(pMsg);

        m_pOrders->StoreOrder(pOrder);
        //std::list<CRomOrder*>::iterator it2 = childList.begin();
        //CRomOrder* pChild = NULL;

        //only one child updates the parent
        //The reason update by child is needed is that body fill/partial fill 'S' message does not have
        //correct last shares!!
        //it2 = childList.begin();

        //if (pOrder->SecType() != ROMVALUE::SECTYPE_COMPLEX)
        //{
            if (childList.size())
            {
                childList[0].pOrder->SetUpdateParent(true);
            }
        //}

        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        databuf_t* pLegBuffer = NULL;

        pBody->getMsgNew(pBuffer);
        databuf_memcpy(pBuffer, ROMVALUE::LEG_SEPRATOR.c_str(), ROMVALUE::LEG_SEPRATOR.size());

        //pack the whole csv string
        for (size_t i = 0; i < pLegs->size(); i++)
        {
            m_pOrders->StoreOrder(childList[i].pOrder);

            pLegBuffer = CBuffStore::getInstance().Get();
            (*pLegs)[i]->getMsgNew(pLegBuffer);
            databuf_memcpy(pBuffer, pLegBuffer->buffer, databuf_unread(pLegBuffer));
            CBuffStore::getInstance().Return(pLegBuffer);
            databuf_memcpy(pBuffer, ROMVALUE::LEG_SEPRATOR.c_str(), ROMVALUE::LEG_SEPRATOR.size());
        }

        databuf_write(pBuffer, "%c", ROMFIELDS::ENDER);

        DoOats(oatsMsgs);

        sent = CDestsManager::getInstance().Send(routeID, ROMVALUE::COMMAND_ORDER, pBuffer->buffer, databuf_unread(pBuffer), error);
        CBuffStore::getInstance().Return(pBuffer);

        if (false == sent)
        {
            for (size_t i = 0; i < pLegs->size(); i++)
            {
                MakeOrderReject(*(childList[i].pCsv), error);
                childList[i].pOrder->NextRomExecTag(omExTag);
                childList[i].pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
                pMsg = new COatsMsg(childList[i].pCsv, childList[i].pOrder, true, true);
                oatsMsgs.push_back(pMsg);
            }

            CCsv* pReject = CCsvStore::getInstance().Get();
            MakeOrderReject(*pReject, *pOrder, error);
            pReject->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            pOrder->NextRomExecTag(omExTag);
            pReject->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
            CClientsManager::getInstance().Send(owner, *pReject, error);
            pMsg = new COatsMsg(pReject, pOrder, false);
            DoOats(pMsg);

            //back out the balance
            BUpdate.orderChange = -1;
            BUpdate.shareChange *= -1;
            BUpdate.dollarChange *= -1;

            CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), BUpdate);
            CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), BUpdate);
        }
    }

    if (sent != true)
    {
        m_pOrders->RemoveOrder(pOrder->RomTag());
        CGStore::getInstance().Return(pOrder);
    }

    for (size_t i = 0; i < pLegs->size(); i++)
    {
        CCsvStore::getInstance().Return(childList[i].pCsv);
        if (sent != true)
        {
            m_pOrders->RemoveOrder(childList[i].pOrder->RomTag());
            CGStore::getInstance().Return(childList[i].pOrder);
        }
    }

    return false;
}

bool CThreadOrderHandler::ValidateCancel(CCsv* pCancel, CRomOrder* pOrder, std::string& err)
{
    bool rc = false;
    int ownercancel = 0;
    pCancel->GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);

    if (ownercancel) //from trader directly
    {
        std::string tag, owner, actTag;
        pCancel->GetAt(ROMFIELDS::CSV_OWNER, owner);
        pCancel->GetAt(ROMFIELDS::CSV_TAG, tag);
        pCancel->GetAt(ROMFIELDS::CSV_NEW_TAG, actTag);

        rc = m_pOrders->GetOrderFromClientTagCancel(owner + tag, actTag, pOrder, err, ownercancel, *this, pCancel);
    }
    else //from OrderMaster
    {
        std::string romtag;
        pCancel->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
        rc = m_pOrders->GetOrderFromRomTagCancel(romtag.c_str(), pOrder, err, ownercancel, *this, pCancel);
    }

    if (true == rc &&
        pOrder->Leave() <= 0)
    {
        rc = false;
        err = "Order already dead";
    }

    return rc;
}

bool CThreadOrderHandler::ProcessClientCancel(CCsv* pCsv)
{
    bool rc = false;

    CRomOrder* pOrder = CGStore::getInstance().Get();
    std::string err;
    std::string owner;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    COatsMsg* pOatsMsg = NULL;
    int ownercancel = 0;
    pCsv->GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);

    if (ValidateCancel(pCsv, pOrder, err) == false)
    {
        MakeCancelReject(*pCsv, err);

        if (ownercancel)
        {
            CClientsManager::getInstance().Send(owner, *pCsv, err);
        }
        if (strlen(pOrder->RomTag()) != 0)//order may not be found
        {
            std::string romExTag;
            if (m_pOrders->GetNextRomExecTag(pOrder->RomTag(), romExTag))
            {
                pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
                pOatsMsg = new COatsMsg(pCsv, pOrder, false, false);
                DoOats(pOatsMsg);
            }
        }
        else
        {
            rc = true;
        }

        if (false == ownercancel)
        {
            struct message m;
            m.data = const_cast<char*>(err.c_str());
            m.len = err.length();
            m.name = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
            m.name_len = CConfig::getInstance().InstanceName().length();
            dart_rolling_log_coming(&m, m.name, m.name_len);
        }
    }
    else
    {
        CancelAddRequiredFields(*pCsv, *pOrder);
        std::string destID = pOrder->Dest();

        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        pCsv->GetMsgNew(pBuffer);

        if (CDestsManager::getInstance().Send(destID, ROMVALUE::COMMAND_CANCEL, pBuffer->buffer, databuf_unread(pBuffer),  err) == false)
        {
            MakeCancelReject(*pCsv, err);

            std::string romExTag;
            m_pOrders->GetNextRomExecTag(pOrder->RomTag(), romExTag);
            pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);

            if (ownercancel)
            {
                CClientsManager::getInstance().Send(owner, *pCsv, err);
                m_pOrders->SetCancelPending(pOrder->RomTag(), false);
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

            COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false, false);
            DoOats(pOatsMsg);
        }
        else
        {
            rc = true;
            CGStore::getInstance().Return(pOrder);
        }
        CBuffStore::getInstance().Return(pBuffer);
    }

    return rc;
}

void CThreadOrderHandler::DoRomCancelPending(CCsv* pCsv, CRomOrder* pOrder)
{
    CancelAddRequiredFields(*pCsv, *pOrder);

    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    pCsv->GetMsgNew(pBuffer);

    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);

    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, true, true);
    DoOats(pOatsMsg);
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
}


bool CThreadOrderHandler::ValidateReplace(CCsv& replace, CRomOrder* pOrder, tBalanceUpdate* pBUpdate, std::string& err)
{
    std::string tag, owner;
    replace.GetAt(ROMFIELDS::CSV_OWNER, owner);
    replace.GetAt(ROMFIELDS::CSV_TAG, tag);

    bool rc = m_pOrders->GetOrderFromClientTagReplace(owner + tag, replace, pOrder, err);

    if (rc)
    {
        char excIns;

        if (replace.GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, excIns) &&
            excIns == ROMVALUE::ISO)
        {
            err = "ISO order is not allowed by ROM system";
            rc = false;
        }
        else
        {
            //if (pOrder->SecType() != ROMVALUE::SECTYPE_COMPLEX)
            //{
                rc = CClientProfileManager::getInstance().AllowedToReplaceOrder(*pOrder, pBUpdate, err);
            //}
            //else
            //{
            //    rc = true;
            //}
        }
    }

    return rc;
}

bool CThreadOrderHandler::ProcessClientReplace(CCsv* pBody)
{
    bool rc = false;

    CRomOrder* pOrder = CGStore::getInstance().Get();
    std::string err;
    COatsMsg* pOatsMsg = NULL;
    tBalanceUpdate BUpdate = {0, 0, 0, ROMVALUE::SECTYPE_EQUITY};
    std::string owner;
    pBody->GetAt(ROMFIELDS::CSV_OWNER, owner);

    if (ValidateReplace(*pBody, pOrder, &BUpdate, err) == false)
    {
        MakeReplaceReject(*pBody, err);
        std::string owner;
        pBody->GetAt(ROMFIELDS::CSV_OWNER, owner);

        CClientsManager::getInstance().Send(owner, *pBody, err);

        if (strlen(pOrder->RomTag()) != 0)//order may not be found
        {
            m_pOrders->ResetReplacingFields(pOrder->RomTag());
            pOatsMsg = new COatsMsg(pBody, pOrder, false, false);
            DoOats(pOatsMsg);
        }
        else
        {
            CGStore::getInstance().Return(pOrder);
            rc = true;
        }

        struct message m;
        m.data = const_cast<char*>(err.c_str());
        m.len = err.length();
        m.name = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
        m.name_len = CConfig::getInstance().InstanceName().length();
        dart_rolling_log_coming(&m, m.name, m.name_len);
    }
    else
    {
        std::string err;
        ReplaceAddRequiredFields(*pBody, *pOrder);
        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        std::string destID = pOrder->Dest();

        pBody->GetMsgNew(pBuffer);

        pBody->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        pBody->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);
        std::string trader = pOrder->Trader();
        pOatsMsg = new COatsMsg(pBody, pOrder, false, true);
        DoOats(pOatsMsg);

        bool sent = CDestsManager::getInstance().Send(destID, ROMVALUE::COMMAND_REPLACE, pBuffer->buffer, databuf_unread(pBuffer), err);

        if (sent == false)
        {
            CCsv* pRplRej = CCsvStore::getInstance().Get();
            pRplRej->Set(pBuffer->buffer, databuf_unread(pBuffer));
            MakeReplaceReject(*pRplRej, err);
            std::string romExTag;

            m_pOrders->GetNextRomExecTag(pOrder->RomTag(), romExTag);
            pRplRej->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);

            CClientsManager::getInstance().Send(owner, *pRplRej, err);
            BUpdate.shareChange *= -1;
            BUpdate.dollarChange *= -1;

            CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), BUpdate);
            CClientProfileManager::getInstance().UpdateBalance(trader, BUpdate);

            //CRomOrder* porder = CGStore::getInstance().Get();

            //std::string romTag;
            //pRplRej->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);

            m_pOrders->SetReplacePending(pOrder->RomTag(), false);
            m_pOrders->ResetReplacingFields(pOrder->RomTag());
            pOatsMsg = new COatsMsg(pRplRej, pOrder, false, false);
            DoOats(pOatsMsg);

            //if (m_pOrders->GetOrderFromRomTag(romTag.c_str(), porder))
            //{
            //    m_pOrders->ResetReplacingFields(porder->RomTag());
            //    pOatsMsg = new COatsMsg(pRplRej, porder, false, false);
            //    DoOats(pOatsMsg);
            //}
            //else
            //{
            //    CCsvStore::getInstance().Return(pRplRej);
            //    CGStore::getInstance().Return(pOrder);
            //}
        }
        else
        {
            m_ReplacingDelts.Add(pOrder->RomTag(), BUpdate);
            CGStore::getInstance().Return(pOrder);
            //pBody->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            //pBody->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);
            //pOatsMsg = new COatsMsg(pBody, pOrder, false, false);
            //DoOats(pOatsMsg);
        }

        CBuffStore::getInstance().Return(pBuffer);
    }

    return rc;
}

bool CThreadOrderHandler::ProcessHostUpdate(CCsv* pCsv)
{
    bool rc = true;
    std::string romtag;
    pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

    CRomOrder* pOrder = CGStore::getInstance().Get();
    std::string err;

    char omTime[22];
    CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
    pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    if (m_pOrders->UpdateOrder(romtag, *pCsv, pOrder, err) == false)
    {
        int status = 0;
        std::string message;
        pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);

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

        return rc;
    }

    if (pOrder->Leave() < 0)
    {
        //generate email to indicate possible overfill
        std::string symbol;
        pCsv->GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
        char sbj[256];
        sprintf(sbj, "Order may have been overfilled in %s(RomTag=%s Symbol=%s) ", CConfig::getInstance().Login().c_str(), romtag.c_str(), symbol.c_str());
        std::string subject = sbj, dumy="";
        CUtility::SendBizEMail(subject, dumy);
    }
    //char omTime[22];
    //CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
    //pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    tDropInfo dropInfo;
    CClientProfileManager::getInstance().GetDropInfo(pOrder->Trader(), dropInfo);
    //CTraderData* pData = CClientProfileManager::getInstance().GetTraderData(theOrder.Trader());

    int cplxtype = 0;
    pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

    if (cplxtype == ROMVALUE::CPLXTYPE_CBOELEG)
    {
        pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, ROMVALUE::CPLXTYPE_LEG);
    }

    if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SINGLE)
    {
        rc = ProcessHostSingleUpdate(pCsv, pOrder, &dropInfo);
    }
    else if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SPREAD)
    {
        rc = ProcessHostContainerUpdate_UserDefinedSpread(pCsv, pOrder, &dropInfo);
    }
    else if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_LEG && //leg of user defined spread
             strlen(pOrder->ParentRomTag()) != 0)
    {
        pCsv->SetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, pOrder->ParentRomTag());
        rc = ProcessHostSingleUpdate(pCsv, pOrder, &dropInfo);
    }
    else if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD &&
             cplxtype == ROMVALUE::CPLXTYPE_LEG)
    {
        rc = ProcessHostLegUpdate_ProductSpread(pCsv, pOrder, &dropInfo);
    }
    else
    {
        rc = ProcessHostContainerUpdate_ProductSpread(pCsv, pOrder, &dropInfo);
    }

    return rc;
}


bool CThreadOrderHandler::ProcessHostLegUpdate_ProductSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData)
{
    bool rc = false;
    std::string err;
    int nStatus = 0;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, nStatus);
    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, pOrder->Exchange());
    pCsv->SetAt(ROMFIELDS::CSV_TAG, pOrder->ClientTag());
    pCsv->SetAt(ROMFIELDS::CSV_PARENT_TAG, pOrder->ParentClientTag());
    pCsv->SetAt(ROMFIELDS::CSV_TRADER, pOrder->Trader());
    pCsv->SetAt(ROMFIELDS::CSV_TRADE_FOR, pOrder->TradeFor());
    pCsv->SetAt(ROMFIELDS::CSV_CAPACITY, pOrder->Capacity());

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
    {
        pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, pOrder->Underlying());
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, pOrder->SecType());
    }

    CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);

    if (CConfig::getInstance().noTPOS() == 0)
    {
        if (((ROMVALUE::STATUS_FILLED == nStatus) ||
                (ROMVALUE::STATUS_PARTIALLYFILLED == nStatus)) &&
                pData->RoninMember)
        {
            char value[200];
            sprintf(value,
                    "%s %s %s %s",
                    pOrder->TradeFor(),
                    pOrder->ClearingAccount(),
                    pOrder->Firm(),
                    pOrder->LocalAccount());
            pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);
            CClientsManager::getInstance().Send("ROMTPOS", *pCsv, err);
        }
    }

    DoDropCopy(*pCsv, pOrder, pData);

    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false, false);
    DoOats(pOatsMsg);

    return rc;
}

bool CThreadOrderHandler::ProcessHostSingleUpdate(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData)
{
    bool rc = false;

    short newstatus = 0;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, newstatus);
    bool copy = false;
    DShares dShares;
    std::string err;

    if (newstatus == ROMVALUE::STATUS_REPLACED)
    {
        if (strlen(pOrder->ParentRomTag()) == 0)
        {
            char newtag[200], oldtag[200];
            sprintf(oldtag, "%s%s", pOrder->Owner(), pOrder->ClientTag());
            sprintf(newtag, "%s%s", pOrder->Owner(), pOrder->NewClientTag());
            m_pOrders->UpdateClientTag(oldtag, newtag);
        }
    }

    // Update Trader Data
    if (strlen(pOrder->ParentRomTag()) == 0) //for userdefined complex order balance is updated only
    {                                        //through parent!!
        UpdateTraderBalance(*pCsv, pOrder);
    }
    else
    {
        if (pOrder->UpdateParent() &&
            (newstatus == ROMVALUE::STATUS_FILLED ||
             newstatus == ROMVALUE::STATUS_PARTIALLYFILLED))
        {
            long lastShares = 0;
            tBalanceUpdate BUpdate = {0, 0, 0, pOrder->SecType()};
            pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);

            CRomOrder* pParent = CGStore::getInstance().Get();
            long parentLastShares = lastShares / pOrder->Ratio();

            m_pOrders->FillParent(pOrder->ParentRomTag(), parentLastShares, BUpdate, pParent);

            CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), BUpdate);
            CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), BUpdate);
            //need to generate body fill with correct values
            std::string omTime;
            pCsv->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
            MakeBodyFill(pParent, parentLastShares, omTime);
        }
    }

    std::string lastshare;
    pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshare);
    if (lastshare.find('.') != std::string::npos)
    {
        //Fractional lastShares
        dShares.Set(*pCsv);
    }

    //pOrder->Update(*pCsv); // Update order regardless what the response is

    //std::string omExTag;
    //pOrder->NextRomExecTag(omExTag);
    //pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

    if (ROMVALUE::STATUS_PENDINGCANCEL != newstatus &&
        ROMVALUE::STATUS_PENDINGNEW != newstatus &&
        ROMVALUE::STATUS_REPLACEPENDING != newstatus)
    {
        if (dShares.active() == true)
        {
            std::string lastShares;
            pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
            std::string leave;
            pCsv->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);
            std::string cum;
            pCsv->GetAt(ROMFIELDS::CSV_CUM_SHARES, cum);

            pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, dShares.Last());
            pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, dShares.Leave());
            pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, dShares.Cum());

            CClientsManager::getInstance().Send(pOrder->Owner(),*pCsv, err);
            //m_pClients->SendWithSeq(pOrder->Owner(), msg);

            pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
            pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);
            pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, cum);
        }
        else
        {
            CClientsManager::getInstance().Send(pOrder->Owner(),*pCsv, err);
        }
    }

    DoDropCopy(*pCsv, pOrder, pData);
    //DoOats(&msg, pOrder, false);
    std::deque<COatsMsg*> oatsMsgs;

    //when ordermaster sends cancelall to "all in one"
    //ROM can receives "out" before it receives cancelall from oats.
    //In that case romcancelpending needs to be made up for
    //Oats purpose.
    if (ROMVALUE::STATUS_CANCELED == newstatus &&
        pOrder->CancelPending() == false)
    {
        std::string romcancel;
        if (pCsv->GetAt(ROMFIELDS::CSV_ROMCANCEL, romcancel) == true &&
            romcancel.compare("1") == 0)
        {
            //make up romcancelpending
            std::string omExTag;
            if (m_pOrders->GetNextRomExecTag(pOrder->RomTag(), omExTag) == false)
            {
                pOrder->NextRomExecTag(omExTag);
            }
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
            copy = true;
            COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, copy, copy);
            copy = false;
            oatsMsgs.push_back(pOatsMsg);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
            pCsv->GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        }
    }

    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, copy, copy);
    copy = true;

    oatsMsgs.push_back(pOatsMsg);
    rc = false;

    if (ROMVALUE::STATUS_FILLED == newstatus &&
        pOrder->CancelPending() == true)
    {
        //generate cancel reject
        std::string text = "Order was filled(too later to cancel)";

        std::string omextag;
        pCsv->GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omextag);
        std::string sStatus;
        pCsv->GetAt(ROMFIELDS::CSV_STATUS, sStatus);
        std::string sText;
        pCsv->GetAt(ROMFIELDS::CSV_TEXT, sText);

        std::string omExTag;
        if (m_pOrders->GetNextRomExecTag(pOrder->RomTag(), omExTag) == false)
        {
            pOrder->NextRomExecTag(omExTag);
        }

        pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, pOrder->ActClientTag());

        MakeCancelReject(*pCsv, text);
        CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
        DoDropCopy(*pCsv, pOrder, pData);
        pOatsMsg = new COatsMsg(pCsv, pOrder, copy, copy);
        oatsMsgs.push_back(pOatsMsg);

        pCsv->SetAt(ROMFIELDS::CSV_STATUS, sStatus);
        pCsv->SetAt(ROMFIELDS::CSV_TEXT, sText);
        pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omextag);
    }

    if (CConfig::getInstance().noTPOS() == 0)
    {
        if (((ROMVALUE::STATUS_FILLED == newstatus) ||
            (ROMVALUE::STATUS_PARTIALLYFILLED == newstatus)) &&
            pData->RoninMember)
        {
            char value[200];
            sprintf(value,
                    "%s %s %s %s",
                    pOrder->TradeFor(),
                    pOrder->ClearingAccount(),
                    pOrder->Firm(),
                    pOrder->LocalAccount());

            std::string acct;
            pCsv->GetAt(ROMFIELDS::CSV_ACCOUNT, acct);
            pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);
            //m_pClients->Send("ROMTPOS", msg.Message());
            CClientsManager::getInstance().Send("ROMTPOS", *pCsv, err);
            pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, acct);
        }
    }

    if (oatsMsgs.size() > 0)
    {
        DoOats(oatsMsgs);
    }

    return rc;
}

void CThreadOrderHandler::MakeBodyFill(CRomOrder* pOrder, long lastShare, const std::string& omTime)
{
    CCsv* pCsv = CCsvStore::getInstance().Get();
    pOrder->GetCsv(*pCsv);
    std::string dummy;
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShare);
    pCsv->SetAt(ROMFIELDS::CSV_LAST_PRICE, pOrder->Price());
    CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, dummy);
    std::string romExTag;
    m_pOrders->GetNextRomExecTag(pOrder->RomTag(), romExTag);
    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
    pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    //need to remove parent from map if it is fully filled
    if (pOrder->Leave() == 0)
    {
        if (pOrder->ReplacePending() == false)
        {
            m_pOrders->RemoveOrder(pOrder->RomTag());
        }
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_FILLED);
    }
    else
    {
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_PARTIALLYFILLED);
    }

    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false, false);
    DoOats(pOatsMsg);
}


bool CThreadOrderHandler::ProcessHostContainerUpdate_ProductSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData)
{
    bool rc = false;
    std::string err;
    short newstatus = 0;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, newstatus);


    if (newstatus == ROMVALUE::STATUS_REPLACED)
    {
        char newtag[200], oldtag[200];
        sprintf(oldtag, "%s%s", pOrder->Owner(), pOrder->ClientTag());
        sprintf(newtag, "%s%s", pOrder->Owner(), pOrder->NewClientTag());
        m_pOrders->UpdateClientTag(oldtag, newtag);
    }

    UpdateTraderBalance(*pCsv, pOrder);

    pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, pOrder->CplxType());

    if (ROMVALUE::STATUS_FILLED != newstatus &&
        ROMVALUE::STATUS_PARTIALLYFILLED != newstatus)
    {
        DoDropCopy(*pCsv, pOrder, pData);
    }

    if (ROMVALUE::STATUS_PENDINGCANCEL != newstatus)
    {
        CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
    }
    
    COatsMsg* pMsg = new COatsMsg(pCsv, pOrder, false, false);
    DoOats(pMsg);

    return rc;
}

bool CThreadOrderHandler::ProcessHostContainerUpdate_UserDefinedSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData)
{
    bool rc = false;
    std::string err;
    short newstatus = 0;
    std::deque<COatsMsg*> oatsMsgs;
    COatsMsg* pMsg = NULL;
    bool copy = false;

    pCsv->GetAt(ROMFIELDS::CSV_STATUS, newstatus);

    if (newstatus == ROMVALUE::STATUS_REPLACED)
    {
        char newtag[200], oldtag[200];
        sprintf(oldtag, "%s%s", pOrder->Owner(), pOrder->ClientTag());
        sprintf(newtag, "%s%s", pOrder->Owner(), pOrder->NewClientTag());
        m_pOrders->UpdateClientTag(oldtag, newtag);
        tBalanceUpdate BUpdate = {0, 0, 0, pOrder->SecType()};
        m_ReplacingDelts.Remove(pOrder->RomTag(), BUpdate);
    }

    //if (pOrder->SecType() != ROMVALUE::SECTYPE_COMPLEX)
    //{
        tBalanceUpdate BUpdate = {0, 0, 0, pOrder->SecType()};

        if (newstatus == ROMVALUE::STATUS_REPLACEREJECTED)
        {
            if (m_ReplacingDelts.Remove(pOrder->RomTag(), BUpdate))
            {
                BUpdate.shareChange *= -1;
                BUpdate.dollarChange *= -1;
                CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), BUpdate);
                CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), BUpdate);
            }
        }
        else if (newstatus == ROMVALUE::STATUS_CANCELED ||
                 newstatus == ROMVALUE::STATUS_ORDERREJECTED ||
                 newstatus == ROMVALUE::STATUS_EXPIRED)
        {
            BUpdate.orderChange = -1;
            long leave = pOrder->Shares() - pOrder->Cum(); //remember that pOrder->Leaves() has already been set to zero!
            BUpdate.shareChange = -1 * pOrder->Ratio() * leave;
            BUpdate.dollarChange = (-1) * leave * pOrder->Price() * pOrder->Multiplier() * pOrder->SideSign() * pOrder->CurrencyRate();
            CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), BUpdate);
            CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), BUpdate);
        }
        /*else if (newstatus == ROMVALUE::STATUS_FILLED)
        {
            tBalanceUpdate BUpdate = {0, 0, 0, pOrder->SecType()};
            if (m_ReplacingDelts.Remove(pOrder->RomTag(), BUpdate))
            {
                BUpdate.shareChange *= -1;
                BUpdate.dollarChange *= -1;
            }
            BUpdate.orderChange = -1;
        }*/
    //}

    if (newstatus == ROMVALUE::STATUS_FILLED)
    {
        if (pOrder->ReplacePending()) //reject replace
        {
            tBalanceUpdate BUpdate = {0, 0, 0, pOrder->SecType()};
            m_ReplacingDelts.Remove(pOrder->RomTag(), BUpdate);

            std::string text, romExTag;
            pCsv->GetAt(ROMFIELDS::CSV_TEXT, text);
            MakeReplaceReject(*pCsv, "Too late to replace");
            COatsMsg* pMsg = new COatsMsg(pCsv, pOrder, copy, copy);

            if (false == copy)
            {
                copy = true;
            }

            oatsMsgs.push_back(pMsg);
            //pOrder->NextRomExecTag(romExTag);
            //pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
            CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
            //pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_FILLED);
            //pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }

        if (pOrder->CancelPending()) //reject cancel
        {
            std::string text, romExTag;
            pCsv->GetAt(ROMFIELDS::CSV_TEXT, text);
            MakeCancelReject(*pCsv, "Too late to cancel");
            COatsMsg* pMsg = new COatsMsg(pCsv, pOrder, copy, copy);
            if (false == copy)
            {
                copy = true;
            }

            oatsMsgs.push_back(pMsg);
            //pOrder->NextRomExecTag(romExTag);
            //pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
            CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
            //pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_FILLED);
            //pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }
    }

    pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, pOrder->CplxType());

    if (ROMVALUE::STATUS_FILLED != newstatus &&
        ROMVALUE::STATUS_PARTIALLYFILLED != newstatus &&
        ROMVALUE::STATUS_PENDINGCANCEL != newstatus)
    {
        DoDropCopy(*pCsv, pOrder, pData);
        CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
        pMsg = new COatsMsg(pCsv, pOrder, copy, copy);
        if (false == copy)
        {
            copy = true;
        }

        oatsMsgs.push_back(pMsg);
    }
    /*else if ((ROMVALUE::STATUS_FILLED == newstatus ||
              ROMVALUE::STATUS_PARTIALLYFILLED == newstatus) &&
             pOrder->SecType() == ROMVALUE::SECTYPE_COMPLEX)
    {
        DoDropCopy(*pCsv, pOrder, pData);
        CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
        pMsg = new COatsMsg(pCsv, pOrder, copy, copy);
        if (false == copy)
        {
            copy = true;
        }

        oatsMsgs.push_back(pMsg);
    }*/

    if (false == copy)
    {
        CGStore::getInstance().Return(pOrder);
        CCsvStore::getInstance().Return(pCsv);
    }
    //CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
    //pMsg = new COatsMsg(pCsv, pOrder, false);
    //oatsMsgs.push_back(pMsg);
    DoOats(oatsMsgs);

    return rc;
}

void CThreadOrderHandler::UpdateTraderBalance(const CCsv& msg, CRomOrder* pOrder)//, CTraderData* pData)
{
    short newstatus = 0;
    msg.GetAt(ROMFIELDS::CSV_STATUS, newstatus);
    char cmd = 'S';
    msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd);

    tBalanceUpdate update = {0, 0, 0, pOrder->SecType()};

    bool doupdate = false;

    if (ROMVALUE::COMMAND_STATUS == cmd)
    {
        if (newstatus == ROMVALUE::STATUS_FILLED ||
                newstatus == ROMVALUE::STATUS_PARTIALLYFILLED)
        {
            double price = 0;
            msg.GetAt(ROMFIELDS::CSV_LAST_PRICE, price);
            long lastshares = 0;
            msg.GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshares);
            doupdate = true;

            if (strlen(pOrder->ParentRomTag()) == 0) //cplx order legs are not included in limit checking!!
            {
                update.dollarChange = -1 * lastshares * pOrder->SideSign() * pOrder->Price() * pOrder->Multiplier() * pOrder->CurrencyRate();
                update.shareChange = -lastshares;
                if (newstatus == ROMVALUE::STATUS_FILLED)
                {
                    update.orderChange = -1;
                }
            }
        }
        else if (newstatus == ROMVALUE::STATUS_REPLACED)
        {
            m_ReplacingDelts.Remove(pOrder->RomTag(), update);
        }
        else if (newstatus == ROMVALUE::STATUS_REPLACEREJECTED)
        {
            if (m_ReplacingDelts.Remove(pOrder->RomTag(), update))
            {
                doupdate = true;
                update.shareChange *= -1;
                update.dollarChange *= -1;
            }
        }
        else if (newstatus == ROMVALUE::STATUS_CANCELED ||
                 newstatus == ROMVALUE::STATUS_EXPIRED ||
                 newstatus == ROMVALUE::STATUS_ORDERREJECTED)
        {
            doupdate = true;
            long sharechange = pOrder->Shares() - pOrder->Cum();
            update.dollarChange =  -1 * pOrder->Price() * sharechange * pOrder->SideSign() * pOrder->Multiplier() * pOrder->CurrencyRate();
            update.shareChange = -1 * sharechange;
            update.orderChange = -1;
        }
    }
    else if (ROMVALUE::COMMAND_ORDER)
    {
        doupdate = true;
        update.dollarChange = pOrder->Price() * pOrder->Leave() * pOrder->Multiplier() * pOrder->CurrencyRate();;
        update.orderChange = 1;
        update.shareChange = pOrder->Leave();
    }

    if (true == doupdate)
    {
        CClientProfileManager::getInstance().UpdateClearingActSecBalance(pOrder->ClearingSecKey(), update);
        CClientProfileManager::getInstance().UpdateBalance(pOrder->Trader(), update);
    }
}

bool CThreadOrderHandler::ProcessDestCancels(const std::string& destID)
{
    bool rc = true;
    std::set<CCsv*> cancels;
    CCsv* pCancel = NULL;
    m_pOrders->BuildDestCancels(destID, cancels);
    std::set<CCsv*>::iterator it = cancels.begin();

    while (it != cancels.end())
    {
        pCancel = *it;
        if (ProcessClientCancel(pCancel))
        {
            CCsvStore::getInstance().Return(pCancel);
        }
        it++;
    }

    return rc;
}


bool CThreadOrderHandler::ProcessClientCancels(CCsv* pCsv, bool dayorderonly)
{
    //std::deque<COatsMsg*> oatsMsgs;
    std::set<CCsv*> cancels;

    int ownercancel = 0;
    pCsv->GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
    CCsv* pCancel = NULL;

    m_pOrders->BuildCancels(*pCsv, cancels, dayorderonly);
    std::set<CCsv*>::iterator it = cancels.begin();

    while (it != cancels.end())
    {
        pCancel = *it;
        pCancel->SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
        if (ProcessClientCancel(pCancel))
        {
            CCsvStore::getInstance().Return(pCancel);
        }
        it++;
    }

    /*
    if (oatsMsgs.size() > 0)
    {
        DoOats(oatsMsgs);
    }

    if (cancels.size() > 0)
    {
        CCsv* pCancel = NULL;
        std::string dest, err;
        std::set<CCsv*>::iterator it = cancels.begin();

        while (it != cancels.end())
        {
            pCancel = *it;
            pCancel->GetAt(ROMFIELDS::CSV_ORDERDEST, dest);

            if (CDestsManager::getInstance().Send(dest, *pCancel, err))
            {
                CCsvStore::getInstance().Return(pCancel);
            }
            else
            {
                CRomOrder* pOrder = CGStore::getInstance().Get();
                std::string romTag, romExTag, errStr;
                pCancel->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);

                if (m_pOrders->GetOrderFromRomTagCancel(romTag.c_str(), pOrder, romExTag, errStr, ownercancel, *this, NULL))
                {
                    if (ownercancel)
                    {
                        m_pOrders->SetCancelPending(romTag, false);
                    }

                    MakeCancelReject(*pCancel, err);
                    pCancel->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
                    COatsMsg* pOatsMsg = new COatsMsg(pCancel, pOrder, false, false);
                    DoOats(pOatsMsg);
                }
                else
                {
                    CCsvStore::getInstance().Return(pCancel);
                    CGStore::getInstance().Return(pOrder);
                }
            }

            it++;
        }
    }*/

    return true;
}

void CThreadOrderHandler::DoOats(COats::tMsgList& msgs)
{
    COats::getInstance().AddMessages(msgs);
}

void CThreadOrderHandler::DoOats(COatsMsg* msg)
{
    COats::getInstance().AddMessage(msg);
}

void CThreadOrderHandler::CancelAddRequiredFields(CCsv& msg, const CRomOrder& Order)
{
    msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, Order.RomTag());
    msg.SetAt(ROMFIELDS::CSV_SIDE, Order.Side());
    msg.SetAt(ROMFIELDS::CSV_SHARES, Order.Shares());
    msg.SetAt(ROMFIELDS::CSV_SYMBOL, Order.Symbol());
}

void CThreadOrderHandler::ReplaceAddRequiredFields(CCsv& msg, const CRomOrder& Order)
{
    msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, Order.RomTag());
    msg.SetAt(ROMFIELDS::CSV_SHARES, Order.Leave() + Order.Cum() + Order.ShareChange());
    msg.SetAt(ROMFIELDS::CSV_SYMBOL, Order.Symbol());
    msg.SetAt(ROMFIELDS::CSV_CAPACITY, Order.Capacity());
    msg.SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, Order.Cum() + Order.Leave() + Order.ShareChange());
    msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, Order.Leave());
    msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, Order.Cum());
    msg.SetAt(ROMFIELDS::CSV_SIDE, Order.Side());
}

void CThreadOrderHandler::MakeReplaceReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadOrderHandler::MakeCancelReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadOrderHandler::MakeOrderReject(CCsv& msg, const std::string& reason)
{
    msg.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, " ");
    msg.SetAt(ROMFIELDS::CSV_AVG_PRICE, "0");
    msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, "0");
    msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, "0");

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadOrderHandler::MakeOrderReject(CCsv& msg, const CRomOrder& order, const std::string& reason)
{
    order.GetCsv(msg);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
    msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void CThreadOrderHandler::DoDropCopy(CCsv& msg, CRomOrder* pOrder, tDropInfo* pData)
{
    if (CConfig::getInstance().noDrop())
    {
        return;
    }

    std::string sOwner = pOrder->Owner();
    int status = 0;
    msg.GetAt(ROMFIELDS::CSV_STATUS, status);

    if (!pData)
    {
        //RomLog::LogMessage("DropCopy error  ", "Trader data not found for " + sOwner);
        return;
    }

    if (pData->ReportingFirm.compare("ROMTPOS") == 0 ||
            (pData->ReportingFirm).empty())//since current db has this value
    {
        return;
    }

    switch (pData->ReportingType)
    {
    case ROMVALUE::DROPCOPY_FILLONLY:
        if (status != ROMVALUE::STATUS_FILLED &&
                status != ROMVALUE::STATUS_PARTIALLYFILLED)
        {
            return;
        }

        break;
    case ROMVALUE::DROPCOPY_ALL:
        break;
    default:
        return;
    }

    msg.SetAt(ROMFIELDS::CSV_CLEARINGID, pOrder->ClearingId());
    msg.SetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, pOrder->ClearingAccount());
    msg.SetAt(ROMFIELDS::CSV_MATURITYDAY, pOrder->MaturityDay());

    if (strlen(pOrder->ClientEcho()))
    {
        msg.SetAt(ROMFIELDS::CSV_CLIENT_ECHO, pOrder->ClientEcho());
    }

    if (msg.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
    {
        msg.SetAt(ROMFIELDS::CSV_UNDERLYING, pOrder->Underlying());
    }


    if (ROMVALUE::STATUS_FILLED == status ||
            ROMVALUE::STATUS_PARTIALLYFILLED == status)
    {
        std::string text = GetDropCopyText(pOrder, msg);
        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    }

    char value[200];
    sprintf(value,
            "%s %s %s %s",
            pOrder->TradeFor(),
            pOrder->ClearingAccount(),
            pOrder->Firm(),
            pOrder->LocalAccount());

    std::string acct;
    msg.GetAt(ROMFIELDS::CSV_ACCOUNT, acct);
    msg.SetAt(ROMFIELDS::CSV_ACCOUNT, value);

    std::string err;

    if (CClientsManager::getInstance().Send(pData->ReportingFirm, msg, err))
    {
        //RomLog::LogMessage("DropCopy To " + pData->ReportingFirm(), msg.Message());
    }
    else
    {
        //RomLog::LogMessage("Can not send DropCopy to " + pData->ReportingFirm(), msg.Message());
    }

    msg.SetAt(ROMFIELDS::CSV_ACCOUNT, acct);
}

std::string CThreadOrderHandler::GetDropCopyText(const CRomOrder* pOrder, CCsv& msg)
{
    int OrderDest = 0;
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST, OrderDest);
    std::string clearingID;
    msg.GetAt(ROMFIELDS::CSV_CLEARINGID, clearingID);
    std::string Purpose = "";
    std::string Exchange = "";
    std::string FTP = "FTP ";
    std::string destName, exdest;
    char sectype = ROMVALUE::SECTYPE_EQUITY;

    msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype);
    msg.GetAt(ROMFIELDS::CSV_EXDEST, exdest);

    CDestsManager::getInstance().GetDestNameFromDestId(pOrder->Dest(), destName);
    if (CRomOrder::IsOptionType(sectype) != true)
    {
        switch(OrderDest)
        {
        case 0: //ARCA
            if (exdest == "N" ||
                    exdest == "A")
            {
                std::string qtag;
                msg.GetAt(ROMFIELDS::CSV_QUERY_TAG, qtag);
                if (qtag == "DOT")
                {
                    Purpose = "FTP SWST";
                    Purpose += exdest;
                }
                else
                {
                    Purpose = FTP + destName;
                }
            }
            else
            {
                //Purpose = "FTP " + CString(COrder::Destination(OrderDest));
                Purpose = FTP + destName;
            }
            msg.GetAt(ROMFIELDS::CSV_EXDEST, Exchange);

            break;
        case 6: //Globex
            Exchange = "GLOBEX";
            Purpose = FTP + Exchange;
            break;
        case 8: //ESP

            if(clearingID == "0690" ||
                    clearingID == "690")
            {
                msg.SetAt(ROMFIELDS::CSV_CONTRA, "NET");
            }
            Purpose = "FTP ESP";

            break;

        default:
            Purpose = FTP + destName;
            Exchange = destName;
            break;
        }
    }
    else
    {
        std::string capacity;
        msg.GetAt(ROMFIELDS::CSV_CAPACITY, capacity);
        Purpose = "FTP ";
        Purpose += destName;

        Purpose += " ";
        int nOpenClose = 0;
        msg.GetAt(ROMFIELDS::CSV_OPENCLOSE, nOpenClose);
        if (nOpenClose == 1)
        {
            Purpose += "O ";
        }
        else
        {
            Purpose += "C ";
        }
        if (capacity == ROMFIELDS::ROM_BROKER_DEALER ||
                capacity == ROMFIELDS::ROM_BROKER_DEALER_FIRM)
        {
            Purpose += "BD";
        }
        else if (capacity == ROMFIELDS::ROM_MARKET_MAKER ||
                 capacity == ROMFIELDS::ROM_MARKET_MAKER_FIRM)
        {
            Purpose += "MM";
        }
        else
        {
            Purpose += "CU";
        }

        if (exdest == "ISE")
        {
            Exchange = "I";
        }
        else if (exdest == "AMEX")
        {
            Exchange = "A";
        }
        else if (exdest == "CBOE")
        {
            Exchange = "C";
        }
        else if (exdest == "PCX")
        {
            Exchange = "P";
        }
        else if (exdest == "PHLX")
        {
            Exchange = "W";
        }
        else if (exdest == "BOX")
        {
            Exchange = "B";
        }
        else
        {
            Exchange = destName;
        }
    }

    int nExchange = 0;
    msg.GetAt(ROMFIELDS::CSV_EXCHANGE, nExchange);
    if (nExchange == 0)
    {
        std::string session;
        msg.GetAt(ROMFIELDS::CSV_TRADING_SESSION, session);
        if (session == "P")
        {
            Exchange = "P";
        }
    }

    if (Exchange == "Direct+")
    {
        Exchange = "N+";
    }

    std::string seprator = ":";
    std::string contra;
    msg.GetAt(ROMFIELDS::CSV_CONTRA, contra);
    return  Purpose + seprator + contra + seprator + Exchange;
}

bool CThreadOrderHandler::ProcessClientHeartBeat(CCsv* pCsv)
{
    //char timeBuffer[22];
    //CUtility::GetGmtTimeStr(timeBuffer, 22);
    //pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, timeBuffer);
    std::string owner;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    std::string err;

    CClientsManager::getInstance().Send(owner, *pCsv, err);
    return true;
}

