#include "Oats.h"
#include "TimerServer.h"
#include "Config.h"
#include "ROMTypedef.h"
#include "client_manager.h"
#include "rom_handler.h"
#include "DestsManager.h"
#include "ClientsManager.h"
#include "ThreadRomOrdersNew.h"
#include "HeavyDuty.h"
#include "BuffStore.h"
#include "debug_logger.h"
#include "Utility.h"
#include "MDSClient.h"
#include "PositionManager.h"


static bool l_live = false;

COats::COats()
{
    l_live = true;
    m_CreateNewEffective = false;
    m_pCon = new CHostConnection("OATS", "OATS", false, true, NULL);

    m_pCon->SetLastLogonTime(GetLastLogonTime());
    m_pCon->SetOpenTime(CConfig::getInstance().StartTime());
    m_pCon->SetServerResetDay(7);
    m_pCon->SetAsyncServer(const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer()));
    m_OrderQueryFinished = false;
}

COats::~COats()
{
    l_live = false;
    if (m_pCon)
    {
        delete m_pCon;
    }
}

time_t COats::GetLastLogonTime()
{
    const std::string& login = CConfig::getInstance().InstanceName();
    const std::string& server = CConfig::getInstance().LastOatsInstancename();

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    time_t rc = CUtility::GetLastLogonTime(login, server, *executor);

    return rc;
}

bool COats::Connect()
{
    bool rt = false;

    init_args* arg = m_pCon->args();
    if (CConfig::getInstance().IsFirstRun() == false ||
        m_CreateNewEffective)
    {
        arg->reset_time->create_new = 0;
        arg->reset_time->last_logon = 0;
    }
    
    if (CConfig::getInstance().Async())
    {
        async_server_con_set* pAsyncServer = const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer());
        if (async_dart_connect(MessageCallback,
                               NULL,
                               0,
                               m_pCon->Port(),
                               const_cast<char*>(m_pCon->IP().c_str()),
                               m_pCon->IP().size(),
                               25,
                               0,
                               7,
                               10,
                               NULL,
                               &ConnectCallback,
                               &parse_rom_message,
                               arg,
                               init_async_rom_logon,
                               &build_offsets,
                               &DummyLowActivityCallback,
                               0,
                               pAsyncServer, m_pCon, CConnection::OnSend) == 0)
        {
            rt = true;
        }
    }
    else
    {
        if (dart_connect(&MessageCallback,
                         NULL,
                         0,
                         m_pCon->Port(),
                         const_cast<char*>(m_pCon->IP().c_str()),
                         m_pCon->IP().size(),
                         25,
                         0,
                         7,
                         10,
                         NULL,
                         &ConnectCallback,
                         &parse_rom_message,
                         m_pCon->args(),
                         &init_rom_logon,
                         &build_offsets,
                         &DummyLowActivityCallback,0) == 0)
        {
            rt = true;
        }
    }

    return rt;
}

void COats::Init()
{
    //while (CConfig::getInstance().Ready() == false)
    //{
    //    CUtility::Sleep(1);
    //}

    m_pCon->SetCloseTime(2359);

    std::string ip;
    int port;
    if (CConfig::getInstance().GetDestIpPort("Oats", ip, port))
    {
        m_pCon->SetIP(ip);
        m_pCon->SetPORT(port);
        tTimerRequest* pReq = new tTimerRequest;
        pReq->timerhandler = &ReconnectCallback;
        pReq->oneshot = false;
        pReq->anyobj = NULL;
        ReConnect();
        CTimerServer::getInstance().AddRequest(pReq);
    }
}

bool COats::ReconnectCallback(time_t now, void* obj)
{
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_TIMER);
    COatsMsg* pMsg = new COatsMsg(pCsv, NULL, false);
    COats::getInstance().AddMessage(pMsg);
    return true;
}

void COats::ReConnect()
{
    if (m_pCon &&
        m_pCon->Connecting() == false &&    //remember: this flag will be turned off by a different thread
        m_pCon->Connected() == false)       //It is crucial to call Connecting() before Connected() to avoid overreconnect
    {
        m_pCon->SetConnecting(true);
        if (Connect() == false)
        {
            m_pCon->SetConnecting(false);
        }
    }
}

void COats::MessageCallback(struct message* m, void* ob)
{
    if (l_live == false)
    {
        return;
    }

    if (m->len - 1 == 0)
    {
        return;
    }

    char* pName = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
    size_t nameLen = CConfig::getInstance().InstanceName().length();
    dart_rolling_log_coming(m, pName, nameLen);
    COats::getInstance().ProcessMessage(m->data, m->len - 1);
}

void COats::ProcessMessage(char* data,  unsigned long len)
{
    char cmd = data[0];

    if (ROMVALUE::COMMAND_CURRENT_STATUS == cmd)
    {
        //The reason to queue orders and store them later is to read off
        //order messages as fast as possible so that oats will not see me
        //being slow consumer!!!
        databuf_t* pBuf = CBuffStore::getInstance().Get();
        databuf_memcpy(pBuf, data, len),
        m_queryResult.insert(pBuf);
    }
    else
    {
        CCsv* pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(data, len);

        char omTime[32];
        CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
        pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

        if (ROMVALUE::COMMAND_ADMIN == cmd)
        {
            short nCmd = 0;

            if (pCsv->GetAt(ROMFIELDS::CSV_COMMAND_FREAK, nCmd))
            {
                pCsv->SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().Instance());
                switch(nCmd)
                {
                case ROMVALUE::QUERY_ORDERDEST_STATUS:
                    ProcessAdminQueryOrderDest(*pCsv);
                    break;
                case ROMVALUE::MODIFY_ORDERDEST_ACCESS:
                    ProcessAdminModifyOrderDestAccess(*pCsv);
                    break;
                case ROMVALUE::BLOCK_USER:
                    ProcessAdminBlockUser(*pCsv);
                    break;
                case ROMVALUE::QUERY_USER_STATUS:
                    ProcessAdminQueryTraderStatus(*pCsv);
                    break;
                case ROMVALUE::CLEARINGACT_SEC_LIMITS_STATUS:
                    ProcessAdminQueryClearingActSecLimitsStatus(*pCsv);
                    break;
                case ROMVALUE::DISCONNECT_USER:
                    ProcessAdminDisconnectUser(*pCsv);
                    break;
                case ROMVALUE::RELOAD_DATABASE:
                    ProcessAdminReloadTraders(*pCsv);
                    break;
                case ROMVALUE::RELOAD_ORDER_DESTINATIONS:
                    ProcessAdminReloadOrderDests(*pCsv);
                    break;
                case ROMVALUE::EXEMPT_TRADER_LIMITS:
                    ProcessAdminExemptTraderLimits(*pCsv);
                    break;
                case ROMVALUE::QUERY_TRADER_LOGONS:
                    ProcessAdminQueryTraderLogons(*pCsv);
                    break;
                case ROMVALUE::DESTROUTE_UPDATE:
                    ProcessAdminDestRouteUpdate(*pCsv);
                    break;
                case  ROMVALUE::QUERY_DESTROUTES:
                    ProcessAdminQueryDestRoutes(*pCsv);
                    break;
                case ROMVALUE::RELOAD_DEST_IPPORT:
                    ProcessAdminReloadDestIpPort(*pCsv);
                    break;
                case ROMVALUE::HOSTCONNECT_STATUS_QUERY:
                    ProcessAdminHostConnectStatusQuery(*pCsv);
                    break;
                case ROMVALUE::CLIENTCONNECT_STATUS_QUERY:
                    ProcessAdminClientConnectStatusQuery(*pCsv);
                    break;
                case ROMVALUE::DISCONNECT_HOST:
                    ProcessAdminDisconnectHost(*pCsv);
                    break;
                case ROMVALUE::DISCONNECT_ROM:
                    ProcessDestDisconnectRom(*pCsv);
                    break;
                case ROMVALUE::RELOAD_CLEARINGACT_SEC_LIMITS:
                    ProcessAdminReloadClearingLimits(*pCsv);
                    break;
                case ROMVALUE::EXEMPT_TRADER_MSGRATE:
                    ProcessAdminExemptTraderMsgRate(*pCsv);
                    break;
                case ROMVALUE::EXEMPT_PRICE_CHECK:
                    ProcessAdminExemptPriceCheck(*pCsv);
                    break;
                case ROMVALUE::BILLINGGRP_LIMIT_STATUS:
                    ProcessAdminQueryBillingGrpLimits(*pCsv);
                    break;
                case ROMVALUE::RELOAD_BILLINGGRP_LIMIT:
                    ProcessAdminReloadBillingGrpLimits(*pCsv);
                    break;
                case ROMVALUE::RELOAD_LENDER_LIMIT:
                    ProcessAdminReloadLenderLimits(*pCsv);
                    break;
                case ROMVALUE::RELOAD_EASYBORROW:
                    ProcessAdminReloadEasyBorrowList(*pCsv);
                    break;
                case ROMVALUE::RELOAD_INIT_POSITION:
                    ProcessAdminReloadInitPosition(*pCsv);
                    break;
                case ROMVALUE::LENDER_LIMIT_QUERY:
                    ProcessAdminQueryLenderLimits(*pCsv);
                    break;
                case ROMVALUE::CLR_LENDING_LIMIT_QUERY:
                    ProcessAdminClrLendingLimitQuery(*pCsv);
                    break;
                case ROMVALUE::POSITION_QUERY:
                    ProcessAdminQueryPosition(*pCsv);
                    break;
                case ROMVALUE::UPDATE_POSITION:
                    ProcessAdminUpdatePosition(*pCsv);
                    break;
                case ROMVALUE::EASYBORROW_QUERY:
                    ProcessAdminQueryEasySymbol(*pCsv);
                    break;
                case ROMVALUE::ADD_EASYSYMBOL:
                    ProcessAdminAddEasySymbol(*pCsv);
                    break;
                //case ROMVALUE::GET_LENDER_GROUPS:
                    //ProcessAdminGetLendingGroups(*pCsv);
                    //ProcessAdminGetLendingGroupsSymbols(*pCsv);
                    //break;
                case ROMVALUE::UPDATE_LENDING_LIMIT:
                    ProcessAdminUpdateLenderLimit(*pCsv);
                    break;
                case ROMVALUE::RELOAD_THRESHOLD:
                    ProcessAdminReloadThresholdList(*pCsv);
                    break;
                case ROMVALUE::THRESHOLD_QUERY:
                    ProcessAdminQueryThresholdSymbol(*pCsv);
                    break;
                case ROMVALUE::ADD_THRESHOLD:
                    ProcessAdminAddThresholdSymbol(*pCsv);
                    break;
                case ROMVALUE::EXEMPT_CLRACT_LIMITS:
                    ProcessAdminExemptClrActLimits(*pCsv);
                    break;
                case ROMVALUE::EXEMPT_BGP_LIMITS:
                    ProcessAdminExemptBgpLimits(*pCsv);
                    break;
                case ROMVALUE::QUERY_FUTURE:
                    ProcessAdminQueryFuture(*pCsv);
                    break;
                case ROMVALUE::RELOAD_FUTURE:
                    ProcessAdminReloadFuture(*pCsv);
                    break;
                default:
                    break;
                }
            }
            CStore<CCsv>::getInstance().Return(pCsv);
            //delete pCsv;
        }
        else
        {
            bool bdelete = false;

            switch (cmd)
            {
            case ROMVALUE::COMMAND_CANCEL:
            {
                std::string romTag;
                pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);
                pCsv->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 0);
                bdelete = ProcessAdminCancel(pCsv);
            }
            break;
            case ROMVALUE::COMMAND_CANCELS:
                ProcessAdminCancels(pCsv);
                break;
            case ROMVALUE::COMMAND_STATUS:
                bdelete = ProcessAdminStatus(pCsv);
                break;
            case ROMVALUE::COMMAND_ZAP:
                bdelete = ProcessAdminZap(pCsv);
                break;
            case ROMVALUE::COMMAND_TEXT:
                ProcessAdminText(*pCsv);
                bdelete = true;
                break;
            //case ROMVALUE::COMMAND_DISCONNECTDEST:
            case ROMVALUE::COMMAND_RELOADDESTCONFIG:
            case ROMVALUE::COMMAND_RESETDESTSEQUENCES:
                {
                    std::string err, dest;
                    pCsv->GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
                    CDestsManager::getInstance().Send(dest, *pCsv, err);
                }
                bdelete = true;
                break;
            case ROMVALUE::COMMAND_ORDER_QUERY_END:
                CThreadRomOrdersNew::getInstance().StoreLiveOrders(m_queryResult);
                CleanQueryResult();
                m_OrderQueryFinished = true;
                send_debug_message("Received all the requested live order request from oats\n");
                PushStats();
                break;
            default:
                bdelete = true;
                break;
            }

            if (bdelete)
            {
                //delete pCsv;
                CStore<CCsv>::getInstance().Return(pCsv);
            }
        }
    }
}

void COats::Connected()
{
    if (m_pCon)
    {
        m_pCon->SetConnected(true);
        m_pCon->SetConnecting(false);
    }
}

void COats::Disconnected()
{
    if (m_pCon)
    {
        m_pCon->SetConnected(false);
        m_pCon->SetLoggedOn(false);
        m_pCon->SetConnecting(false);
    }

    //in case in the middle of querying live orders
    CleanQueryResult();
}

bool COats::Send(const char* pBuf, int len)
{
    bool rc = true;

    if (m_pCon->Connected())
    {
        if (m_pCon->Send(const_cast<char*>(pBuf), len) == false)
        {
            rc = false;
        }
    }
    else
    {
        rc = false;
    }

    return rc;
}

void COats::ConnectCallback(con_obj* co, int is_loggedon, char* log, int len)
{
    if (l_live == false)
    {
        return;
    }

    COats::getInstance().SetCreateNewEffective(true);

    if (1 == is_loggedon)
    {
        struct message msg;
        msg.data = log;
        msg.len = len;
        msg.name = const_cast<char*>(get_name(co));
        msg.name_len = strlen(get_name(co));
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());

        COats::getInstance().LoggedOn();
        //CConfig::getInstance().SetOatsSessiopnContinue(false);

        if (CConfig::getInstance().QueryLiveOrderFromOats() &&
            COats::getInstance().OrderQueryFinished() == false)
        {
            CCsv csvMsg;
            csvMsg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER_QUERY);

            std::string sMsg;
            csvMsg.GetMsg(sMsg);
            COats::getInstance().Send(sMsg.c_str(), sMsg.size());
            send_debug_message("Sent querying live order request to oats\n");
        }
        else
        {
            //OrderMaster may miss the chance to get the following since it may logon to Oats earlier than ROM does so push the following to it
            //also need do this if QueryLiveOrderFromOats is on!!!
            COats::getInstance().PushStats();
        }
    }
    else if (2 == is_loggedon)
    {
        COats::getInstance().Connected();
    }
    else
    {
        COats::getInstance().Disconnected();
    }
}


void COats::PushStats()
{
    CCsv csvMsg;

    csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::QUERY_TRADER_LOGONS);
    ProcessAdminQueryTraderLogons(csvMsg);

    csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::QUERY_DESTROUTES);
    ProcessAdminQueryDestRoutes(csvMsg);

    csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::HOSTCONNECT_STATUS_QUERY);
    ProcessAdminHostConnectStatusQuery(csvMsg);

    csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::CLIENTCONNECT_STATUS_QUERY);
    ProcessAdminClientConnectStatusQuery(csvMsg);
}

void COats::LoggedOn()
{
    if (m_pCon)
    {
        time_t tm_t = 0;
        time(&tm_t);
        m_pCon->SetLoggedOn(true);
        m_pCon->SetConnected(true);
        m_pCon->SetLastLogonTime(tm_t);
        CConfig::getInstance().UpdateLastOatsInstanceName(CConfig::getInstance().OatsInstancename());
    }
}

bool COats::ProcessMessage(tMsgList& msglist)
{
    bool rc = true;
    tMsgListIt it = msglist.begin();
    COatsMsg* pMsg = NULL;

    while (it != msglist.end())
    {
        pMsg = *it;

        switch(pMsg->cmd())
        {
        case ROMVALUE::COMMAND_RESPONSETOADMIN:
            m_pCon->SendCmd(pMsg->msg());
            break;
        case ROMVALUE::COMMAND_ORDER:
        case ROMVALUE::COMMAND_STATUS:
        case ROMVALUE::COMMAND_CANCEL:
        case ROMVALUE::COMMAND_REPLACE:
        case ROMVALUE::COMMAND_CANCELS:
        {
            databuf_t* pBuffer = CBuffStore::getInstance().Get();
            BuildOatsMsg(pBuffer, pMsg);
            m_pCon->Send(pBuffer->buffer, databuf_unread(pBuffer), false);
            CBuffStore::getInstance().ReturnB(pBuffer);
        }
        break;
        case ROMVALUE::COMMAND_ADMIN:
            //process the request received directly from Admin
            break;
        case ROMVALUE::COMMAND_TIMER:
            ReConnect();
            break;
        default:
            break;
        }

        it++;
        delete pMsg;
    }

    return rc;
}

void COats::ProcessAdminQueryOrderDest(CCsv& msg)
{
    std::string destRtID;
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, destRtID);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());

    ROMTYPEDEF::tConnectionStatus conStatus;
    std::string msgs;

    if (CDestsManager::getInstance().GetDestStatus(destRtID, conStatus) == false)
    {
        char value[100];
        sprintf(value, "%s:Invalid OrderDest:%s", (CConfig::getInstance().Login()).c_str(), destRtID.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(msgs);
    }
    else
    {
        char value[100];
        if (conStatus.connected == true)
        {
            sprintf(value, "%s:%s is Connected To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        else
        {
            sprintf(value, "%s:%s is Not Connected To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        std::string message;
        msg.GetMsg(message);
        msgs = message;

        if (conStatus.loggedon == true)
        {
            sprintf(value, "%s:%s is Logged On To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        else
        {
            sprintf(value, "%s:%s is Not Logged On To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(message);
        msgs += message;

        if (conStatus.blocked == false)
        {
            sprintf(value, "%s:Users are Allowd To Place Orders Into %s", (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        else
        {
            sprintf(value, "%s:Users are Not Allowd To Place Orders Into %s", (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(message);
        msgs += message;
    }

    m_pCon->SendCmd(msgs);

}

void COats::ProcessAdminModifyOrderDestAccess(CCsv& msg)
{
    std::string destRtID = "-1";
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, destRtID);

    //ROMTYPEDEF::tConnectionStatus conStatus;
    //CDestsManager::getInstance().GetDestStatus(destRtID, conStatus);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);
    //msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    //msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    std::string eMsg = CConfig::getInstance().Login();
    char value[200];

    if (action)
    {
        CDestsManager::getInstance().Block(destRtID);
    }
    else
    {
        CDestsManager::getInstance().UnBlock(destRtID);
    }

    if (destRtID.compare("-1") == 0)
    {
        if (action)
        {
            sprintf(value, "%s:Users are not allowed to place orders into any destination", (CConfig::getInstance().Login()).c_str());
            eMsg += std::string(":All Order Destinations are blocked");
        }
        else
        {
            sprintf(value, "%s:Users are allowed to place orders into all destinations", (CConfig::getInstance().Login()).c_str());
            eMsg += std::string(":All Order Destinations are unblocked");
        }
        CUtility::SendBizEMail(eMsg, eMsg);
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    }
    else
    {
        CHostConnection* pCon = NULL;
        CDestsManager::getInstance().GetConnection(destRtID, pCon);
        
        if (NULL == pCon)
        {
            sprintf(value, "%s:Invalid Dest:%s", (CConfig::getInstance().Login()).c_str(), destRtID.c_str());
            msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        }
        else
        {
            /*if (pCon->CloseTime() <= CUtility::GetLocalTimeHHMM() &&
                    false == action)
            {
                sprintf(value, "%s:Can not unblock %s(has passed Sesson End Time %d)", (CConfig::getInstance().Login()).c_str(), conStatus.description.c_str(), pCon->CloseTime());
                msg.SetAt(ROMFIELDS::CSV_ACTION_FREAK, 1);
            }
            else
            {*/
                /*if (action)
                {
                    CDestsManager::getInstance().Block(destRtID);
                    eMsg = CConfig::getInstance().Login() + std::string(":") + pCon->Description() + std::string(" has been blocked");
                    CUtility::SendBizEMail(eMsg, eMsg);
                    sprintf(value, "%s:Users are not allowed to place orders into order destination:%s", (CConfig::getInstance().Login()).c_str(), pCon->Description().c_str());
                }
                else
                {
                    CDestsManager::getInstance().UnBlock(destRtID);
                    eMsg = CConfig::getInstance().Login() + std::string(":") + pCon->Description() + std::string(" has been unblocked");
                    CUtility::SendBizEMail(eMsg, eMsg);
                    sprintf(value, "%s:Users are allowed to place orders into order destination:%s", (CConfig::getInstance().Login()).c_str(), pCon->Description().c_str());
                }*/
            //}
        }

        //msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    }

    //std::string message;
    //msg.GetMsg(message);
    //m_pCon->SendCmd(message);
}

void COats::ProcessAdminQueryClearingActSecLimitsStatus(CCsv& msg)
{
    std::string ClearingActSec;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, ClearingActSec);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    tLimits limits;
    std::string message;

    if (CClientProfileManagerNew::getInstance().GetClearingActSecLimits(ClearingActSec, limits))
    {
        if (limits.exempted)
        {
            char text[128];
            sprintf(text, "ClearingActSec %s's limits check is exempted", ClearingActSec.c_str());
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        }
        msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, limits.MaxOrders);
        msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, limits.NumOrders);
        msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, limits.MaxTotalShares);
        msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, limits.NumShares);
        msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, limits.BuyingPower, 2);
        char balanceStr[128];
        sprintf(balanceStr, "%f|%f", limits.Balance[ROMVALUE::BUY], limits.Balance[ROMVALUE::SELL]);
        msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
        msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, limits.MaxShares);
        msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, limits.MaxDollars, 2);
        msg.GetMsg(message);
    }
    else
    {
        char value[200];
        sprintf(value, "%s:%s is not valid", (CConfig::getInstance().Login()).c_str(), ClearingActSec.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(message);
    }

    m_pCon->SendCmd(message);
}


void COats::ProcessAdminQueryTraderStatus(CCsv& msg)
{
    std::string trader;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);
    char secType = 'N';
    msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType);
    std::string product;
    msg.GetAt(ROMFIELDS::CSV_SYMBOL, product);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());

    std::string msgs;
    char value[200];
    std::string message;

    CTraderData traderData(trader);

    if (CClientProfileManagerNew::getInstance().GetTraderData(trader, &traderData))
    {
        if (traderData.Exempt())
        {
            sprintf(value, "Trader %s's Limits Check is exempted", trader.c_str());
            msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        }
        
        char secTypes[4] = {ROMVALUE::SECTYPE_EQUITY, ROMVALUE::SECTYPE_OPTION, ROMVALUE::SECTYPE_OPTIONFUTURE, ROMVALUE::SECTYPE_FUTURE};
        msg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::TRADER_LIMITS_STATUS);

        if (secType != 'N')
        {
            int index = 0;
            switch(secType)
            {
            case ROMVALUE::SECTYPE_EQUITY:
                index = ROMVALUE::EQUITY;
                break;
            case ROMVALUE::SECTYPE_OPTION:
                index = ROMVALUE::OPTION;
                break;
            case ROMVALUE::SECTYPE_OPTIONFUTURE:
                index = ROMVALUE::OPTIONONFUTURE;
                break;
            case ROMVALUE::SECTYPE_FUTURE:
                index = ROMVALUE::FUTURE;
                break;
            }
            //if (ROMVALUE::FUTURE != index ||
            //    CConfig::getInstance().FutureProductLimitEnabled() == 0 ||
            //    (CConfig::getInstance().FutureProductLimitEnabled() == 1 &&
            //     traderData.FutureProductReady() == false))
            if (ROMVALUE::FUTURE != index ||
                (ROMVALUE::FUTURE == index && traderData.HasFutureProductLimits() == false))
            {
                msg.SetAt(ROMFIELDS::CSV_SYMBOL, "");
                msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, traderData.MaxOrders(index));
                msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, traderData.NumOrders(index));
                msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, traderData.MaxTotalShares(index));
                msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, traderData.NumShares(index));
                msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, traderData.BuyingPower(index), 2);
                char balanceStr[128];
                sprintf(balanceStr, "%f|%f", traderData.Balance(index, ROMVALUE::BUY), traderData.Balance(index, ROMVALUE::SELL));
                msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
                msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, traderData.MaxShares(index));
                msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, traderData.MaxDollars(index), 2);
                msg.SetAt(ROMFIELDS::CSV_MSGRATE_LIMIT, traderData.msgrateLimit(index));
                msg.GetMsg(message);
                msgs += message;
            }
            else
            {
                tLimits limits;
                if (traderData.GetFutureLimits(product, limits))
                {
                    msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, limits.MaxOrders);
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, limits.NumOrders);
                    msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, limits.MaxTotalShares);
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, limits.NumShares);
                    msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, limits.BuyingPower, 2);
                    char balanceStr[128];
                    sprintf(balanceStr, "%f|%f", limits.Balance[ROMVALUE::BUY], limits.Balance[ROMVALUE::SELL]);
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
                    msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, limits.MaxShares);
                    msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, limits.MaxDollars, 2);
                }
                msg.SetAt(ROMFIELDS::CSV_MSGRATE_LIMIT, traderData.msgrateLimit(index));
                msg.GetMsg(message);
                msgs += message;
            }
        }
        else
        {
            for (int i = 0; i < ROMVALUE::SECNUM; i++)
            {
                msg.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, secTypes[i]);
                //if (ROMVALUE::FUTURE != i ||
                //    CConfig::getInstance().FutureProductLimitEnabled() == 0 ||
                //    (CConfig::getInstance().FutureProductLimitEnabled() == 1 &&
                //     traderData.FutureProductReady() == false))
                if (ROMVALUE::FUTURE != i ||
                    (ROMVALUE::FUTURE == i && traderData.HasFutureProductLimits() == false))
                {
                    msg.SetAt(ROMFIELDS::CSV_SYMBOL, "");
                    msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, traderData.MaxOrders(i));
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, traderData.NumOrders(i));
                    msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, traderData.MaxTotalShares(i));
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, traderData.NumShares(i));
                    msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, traderData.BuyingPower(i), 2);
                    char balanceStr[128];
                    sprintf(balanceStr, "%f|%f", traderData.Balance(i, ROMVALUE::BUY), traderData.Balance(i, ROMVALUE::SELL));
                    msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
                    msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, traderData.MaxShares(i));
                    msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, traderData.MaxDollars(i), 2);
                    msg.SetAt(ROMFIELDS::CSV_MSGRATE_LIMIT, traderData.msgrateLimit(i));
                    msg.GetMsg(message);
                    msgs += message;
                }
                else
                {
                    CRomMap<tLimits*> map;
                    traderData.GetFutureLimits(map);
                    std::set<std::string> products;
                    map.GetAllKeys(products);

                    //allowed products
                    std::string symbol;
                    std::set<std::string>::iterator it = products.begin();
                    while (it != products.end())
                    {
                        symbol.append(*it);
                        symbol.append("/");
                        it++;
                    }

                    if (symbol.empty())
                    {
                        msg.SetAt(ROMFIELDS::CSV_FUTURE_PRODUCTS, "/");
                        msg.GetMsg(message);
                        msgs += message;
                    }
                    else
                    {
                        //msg.SetAt(ROMFIELDS::CSV_FUTURE_PRODUCTS, symbol);
                        //msg.GetMsg(message);
                        //msgs += message;

                        msg.SetAt(ROMFIELDS::CSV_FUTURE_PRODUCTS, "");
                        //their limits
                        std::list<CRomMap<tLimits*>::tKeyValuePair> keyvalues;
                        map.GetKeyValuePairs(keyvalues);
                        std::list<CRomMap<tLimits*>::tKeyValuePair>::iterator it1 = keyvalues.begin();
                        while (it1 != keyvalues.end())
                        {
                            msg.SetAt(ROMFIELDS::CSV_SYMBOL, it1->key);
                            msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, (it1->value)->MaxOrders);
                            msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, (it1->value)->NumOrders);
                            msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, (it1->value)->MaxTotalShares);
                            msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, (it1->value)->NumShares);
                            msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, (it1->value)->BuyingPower, 2);
                            char balanceStr[128];
                            sprintf(balanceStr, "%f|%f", (it1->value)->Balance[ROMVALUE::BUY], (it1->value)->Balance[ROMVALUE::SELL]);
                            msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
                            msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, (it1->value)->MaxShares);
                            msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, (it1->value)->MaxDollars, 2);

                            delete it1->value;
                            it1++;

                            if (it1 != keyvalues.end())
                            {
                                msg.SetAt(ROMFIELDS::CSV_MSGRATE_LIMIT, "");
                            }
                            else
                            {
                                msg.SetAt(ROMFIELDS::CSV_MSGRATE_LIMIT, traderData.msgrateLimit(i));
                            }

                            msg.GetMsg(message);
                            msgs += message;

                        }
                    }
                }
            }
        }
    }
    else
    {
        sprintf(value, "%s:User %s Does NOT Have a ROM Account", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(message);
        msgs = message;
    }

    m_pCon->SendCmd(msgs);
}

void COats::ProcessAdminDisconnectUser(CCsv& msg)
{
    std::string trader;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientsManager::getInstance().Find(trader))
    {
        CClientsManager::getInstance().Disconnect(trader);
        sprintf(value, "%s:User %s has been disconnected", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    }
    else
    {
        sprintf(value, "%s:User %s is not logged on", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    }

    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminReloadTraders(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    char text[512];

    tDuty duty = {NULL, NULL, false};
    duty.theFunc = CClientProfileManagerNew::ReloadProfileFromDB;
    std::string* pStr = new std::string;
    std::string message;

    if (msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, *pStr))
    {
        duty.pArg = pStr;
        sprintf(text, "%s:Loading Trader(%s) Information...",
                CConfig::getInstance().Login().c_str(),
                pStr->c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading Trader(%s) Information",
                CConfig::getInstance().Login().c_str(),
                pStr->c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);
    }
    else
    {
        sprintf(text, "%s:Loading All Traders Information...",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading All Traders Information",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        duty.pArg = NULL;
        delete pStr;
    }
    CHeavyDuty::getInstance().AddMessage(duty);
}

void COats::ProcessAdminReloadOrderDests(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_TEXT, CConfig::getInstance().Login() + ":Loading Order Destinations...");
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);

    tDuty duty = {NULL, NULL, false};
    duty.theFunc = CDestsManager::ReloadDestsFromDB;
    std::string* pStr = new std::string;
    if (msg.GetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, *pStr))
    {
        duty.pArg = pStr;
    }
    else
    {
        duty.pArg = NULL;
        delete pStr;
    }
    CHeavyDuty::getInstance().AddMessage(duty);

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, CConfig::getInstance().Login() + ":...Done Loading Destinations");
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminExemptTraderLimits(CCsv& msg)
{
    std::string trader;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientProfileManagerNew::getInstance().ExemptTrader(trader, action))
    {
        if (action == 1)
        {
            sprintf(value, "%s:User %s's Limits Check has been exempted", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }
        else
        {
            sprintf(value, "%s:User %s's Limits Check has been enforced", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }
    }
    else
    {
        sprintf(value, "%s:User %s Not Found", (CConfig::getInstance().Login()).c_str(), trader.c_str());
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminExemptPriceCheck(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);
    char secType = ROMVALUE::SECTYPE_EQUITY;
    msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    char text[516];

    if (ROMVALUE::SECTYPE_EQUITY == secType)
    {
        std::string symbol;
        msg.GetAt(ROMFIELDS::CSV_SYMBOL_FREAK, symbol);
        CMDSClient::getInstance().SetExempted(symbol, (bool)action);
         if (action == 1)
        {
            if (symbol.empty())
            {
                strcpy(text, "Equity Price check has been exempted for all symbols");
            }
            else
            {
                sprintf(text, "Equity Price check has been exempted for symbol:%s", symbol.c_str());
            }
        }
        else
        {
            if (symbol.empty())
            {
                strcpy(text, "Equity Price check has been enforced for all symbols");
            }
            else
            {
                sprintf(text, "Equity Price check has been enforced for symbol:%s", symbol.c_str());
            }
        }
    }
    else
    {
        std::string trader;
        msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
        CMDSClient::getInstance().SetTraderExempt(trader, (bool)action);

        if (action == 1)
        {
            if (trader.empty())
            {
                strcpy(text, "Option Price check has been exempted for all traders");
            }
            else
            {
                sprintf(text, "Option Price check has been exempted for trader:%s", trader.c_str());
            }
        }
        else
        {
            if (trader.empty())
            {
                strcpy(text, "Option Price check has been enforced for all traders");
            }
            else
            {
                sprintf(text, "Option Price check has been enforced for trader:%s", trader.c_str());
            }
        }
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminExemptTraderMsgRate(CCsv& msg)
{
    std::string trader;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientsManager::getInstance().SetMsgRateExempt(trader, (bool)action))
    {
        if (action == 1)
        {
            sprintf(value, "%s:User %s's Message Rate Limit is exempted", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }
        else
        {
            sprintf(value, "%s:User %s's Message Rate Limit is enforced", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }
    }
    else
    {
        sprintf(value, "%s:User %s Not Found", (CConfig::getInstance().Login()).c_str(), trader.c_str());
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminQueryTraderLogons(CCsv& msg)
{
    std::string logins;
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    CClientsManager::getInstance().GetLoggedOnLogins(logins);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_TRADER_FREAK, logins);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminReloadDestIpPort(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    CCsv response;
    std::string clientfreak;
    msg.GetAt(ROMFIELDS::CSV_CLIENT_FREAK, clientfreak);
    response.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    response.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::RELOAD_DEST_IPPORT);
    response.SetAt(ROMFIELDS::CSV_CLIENT_FREAK, clientfreak);
    response.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());

    char value[100];

    std::string destName;
    std::string destID;
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, destID);

    CDestsManager::getInstance().ReloadIpPort(destID);

    ROMTYPEDEF::tConnectionStatus conStatus;
    if (CDestsManager::getInstance().GetDestStatus(destID, conStatus))
    {
        sprintf(value, "%s:Done Reload IP/PORT of %s", CConfig::getInstance().Login().c_str(), conStatus.description.c_str());
    }
    else
    {
        sprintf(value, "%s:Invalid DestID %s", CConfig::getInstance().Login().c_str(), destID.c_str());
    }
    response.SetAt(ROMFIELDS::CSV_TEXT, value);

    std::string message;
    //msg.GetMsg(message);
    response.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminDestRouteUpdate(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);
    std::string exchange, dest;
    msg.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange);
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
    if (CDestsManager::getInstance().UpdateDestRoute(exchange, dest) == false)
    {
        std::string text = "Invalid new route ID:" + dest;
        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    }
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminQueryDestRoutes(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string destRoutes = CDestsManager::getInstance().GetDestRoutes();
    msg.SetAt(ROMFIELDS::CSV_ORDERDEST, destRoutes);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}


void COats::ProcessAdminHostConnectStatusQuery(CCsv& msg)
{
    std::string adminID;
    msg.GetAt(ROMFIELDS::CSV_CLIENT_FREAK, adminID);

    std::list<CHostConnection*> conList;
    CDestsManager::getInstance().GetAllConnections(conList);
    std::list<CHostConnection*>::iterator it = conList.begin();
    CHostConnection* pCon = NULL;

    while (it != conList.end())
    {
        pCon = *it;
        HostConnectionUpdate(*pCon, adminID);
        //ExchangeConnectionUpdate(*pCon, adminID);
        it++;
    }
}

/*
void COats::ExchangeConnectionUpdate(CHostConnection& con, std::string adminID)
{
    CCsv* pCsv = CCsvStore::getInstance().Get();

    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::EXCHANGEINTERFACE_STATUS_QUERY);
    pCsv->SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    pCsv->SetAt(ROMFIELDS::CSV_CLIENT_FREAK, adminID);

    pCsv->SetAt(ROMFIELDS::CSV_ACTION_FREAK, con.ExchangeLoggedOn());

    char timeBuffer[22];
    CUtility::GetGmtTimeStr(timeBuffer, 22);
    pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, timeBuffer);
    pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, con.Name());

    char value[50];
    sprintf(value, "%s | %d", con.IP().c_str(), con.Port());

    pCsv->SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    pCsv->GetMsg(message);
    m_pCon->SendCmd(message);

    CCsvStore::getInstance().Return(pCsv);
}
*/
void COats::HostConnectionUpdate(CHostConnection& con, std::string adminID)
{
    CCsv msg;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::HOSTCONNECT_STATUS_QUERY);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_CLIENT_FREAK, adminID);

    char value[30];
    sprintf(value, "%d|%d|%d|%d", con.Connected(), con.LoggedOn(), con.Blocked(), con.ExchangeLoggedOn());

    msg.SetAt(ROMFIELDS::CSV_ACTION_FREAK, value);

    //time_t t = 0;
    //time(&t);
    msg.SetAt(ROMFIELDS::CSV_OM_TIME, con.LogonTime());
    msg.SetAt(ROMFIELDS::CSV_ORDERDEST, con.Name());

    sprintf(value, "%s | %d", con.IP().c_str(), con.Port());

    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminClientConnectStatusQuery(CCsv& msg)
{
    std::list<CClientConnection*> conList;
    CClientsManager::getInstance().GetAllConnections(conList);

    std::string adminID;
    msg.GetAt(ROMFIELDS::CSV_CLIENT_FREAK, adminID);
    std::list<CClientConnection*>::iterator it = conList.begin();
    CClientConnection* pCon = NULL;

    while (it != conList.end())
    {
        pCon = *it;
        if (pCon->Connected())
        {
            ClientConnectionUpdate(*pCon, adminID);
        }
        it++;
    }
}

void COats::ClientConnectionUpdate(CClientConnection& con, std::string adminID)
{
    CCsv msg;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::CLIENTCONNECT_STATUS_QUERY);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_CLIENT_FREAK, adminID);

    char value[30];
    sprintf(value, "%d|%d|%d", con.Connected(), con.LoggedOn(), con.Blocked());

    msg.SetAt(ROMFIELDS::CSV_ACTION_FREAK, value);

    //time_t t = 0;
    //time(&t);
    msg.SetAt(ROMFIELDS::CSV_OM_TIME, con.LogonTime());
    msg.SetAt(ROMFIELDS::CSV_TRADER_FREAK, con.Name());

    sprintf(value, "%s | %d", con.IP().c_str(), con.Port());

    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    if (m_pCon)
    {
        std::string message;
        msg.GetMsg(message);
        m_pCon->SendCmd(message);
    }
}


void COats::ProcessAdminReloadClearingLimits(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    char text[256];

    tDuty duty = {NULL, NULL, false};
    duty.theFunc = CClientProfileManagerNew::ReloadClearingActSecLimits;
    std::string* pStr = new std::string;
    std::string message;

    if (msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, *pStr))
    {
        duty.pArg = pStr;
        sprintf(text, "%s:Loading ClearingActLimits(%s) Information...",
                CConfig::getInstance().Login().c_str(),
                pStr->c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading ClearingActLimits(%s) Information",
                CConfig::getInstance().Login().c_str(),
                pStr->c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);
    }
    else
    {
        sprintf(text, "%s:Loading All ClearingActLimits Information...",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading All ClearingActLimits Information",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        duty.pArg = NULL;
        delete pStr;
    }

    CHeavyDuty::getInstance().AddMessage(duty);
}

void COats::ProcessAdminDisconnectHost(CCsv& msg)
{
    if (msg.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST_FREAK) == false)
    {
        std::string destID;
        msg.GetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, destID);
        std::string destName;
        if (CDestsManager::getInstance().GetDestNameFromDestId(destID, destName))
        {
            CDestsManager::getInstance().Disconnect(destName);
        }
    }
}

void COats::ProcessAdminBlockUser(CCsv& msg)
{
    std::string trader;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, trader);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientProfileManagerNew::getInstance().FindTrader(trader))
    {
        if (action == 1)
        {
            sprintf(value, "%s:User %s has been blocked", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }
        else
        {
            sprintf(value, "%s:User %s has been unblocked", (CConfig::getInstance().Login()).c_str(), trader.c_str());
        }

        CClientConnection* pCon = NULL;
        if (CClientsManager::getInstance().GetConnection(trader, pCon))
        {
            pCon->SetBlocked(action);
            ClientConnectionUpdate(*pCon, "");
        }
    }
    else
    {
        sprintf(value, "%s:User %s Not Found", (CConfig::getInstance().Login()).c_str(), trader.c_str());
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessDestDisconnectRom(CCsv& msg)
{
    std::string destID = "0";
    msg.GetAt(ROMFIELDS::CSV_ORDERDEST, destID);
    std::string dummy;
    CDestsManager::getInstance().Send(destID, msg, dummy);
}

void COats::BuildOatsMsg(databuf_t* pBuffer, COatsMsg* pOatsMsg)
{
    CRomOrderNew* pOrder = pOatsMsg->Order();

    if (pOrder == NULL)
    {
        pOatsMsg->csv()->GetMsgNew(pBuffer);
        return;
    }

    CCsv OatsMsg;//, Msg(pOatsMsg->msg());
    CCsv* pMsg = pOatsMsg->csv();

    /*
    if ((pOrder->Status() == ROMVALUE::STATUS_EXPIRED ||
         pOrder->Status() == ROMVALUE::STATUS_DONEFORDAY) &&
        pOrder->Leave() != 0)
    {
        //Don't fool the oats database
        //Those orders are still live!!
        return;
    }
    */

    pOrder->GetCsv(OatsMsg);
    OatsMsg.SetAt(ROMFIELDS::CSV_OATSMULTIPLIER, pOrder->Multiplier(), 2);

    int cplxType = -1;
    int type = ROMVALUE::TYPE_LIMIT;
    pMsg->GetAt(ROMFIELDS::CSV_TYPE, type);

    if (pMsg->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType) == true)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType);
    }

    std::string exDest;
    if (pMsg->GetAt(ROMFIELDS::CSV_EXDEST, exDest) == true)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_EXDEST, exDest);
    }

    std::string omExTag;
    pMsg->GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
    OatsMsg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

    char cmd = ROMVALUE::COMMAND_ORDER;
    pMsg->GetAt(ROMFIELDS::CSV_COMMAND, cmd);
    OatsMsg.SetAt(ROMFIELDS::CSV_COMMAND, cmd);

    int status = -1;
    pMsg->GetAt(ROMFIELDS::CSV_STATUS, status);

    std::string mktAccnt;
    if (pMsg->GetAt(ROMFIELDS::CSV_MM_ACCOUNT, mktAccnt))
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_MM_ACCOUNT, mktAccnt);
    }

    std::string sTime;
    pMsg->GetAt(ROMFIELDS::CSV_TIME, sTime);

    if (status == ROMVALUE::STATUS_ROMCANCELPENDING ||
        status == ROMVALUE::STATUS_ROMREPLACEPENDING ||
        cmd == ROMVALUE::COMMAND_ORDER)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_TIME, sTime);
    }

    if (CUtility::IsFutureType(pOrder->SecType()))
    {
        std::string instruction;
        OatsMsg.GetAt(ROMFIELDS::CSV_INSTRUCTIONS, instruction);
        OatsMsg.SetAt(ROMFIELDS::CSV_ACCOUNT, instruction);
    }

    long lastShares = 0;
    pMsg->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
    if (cmd == ROMVALUE::COMMAND_STATUS )
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_COMMAND, cmd);
        OatsMsg.SetAt(ROMFIELDS::CSV_STATUS, status);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_CLIENT_TIME, sTime);

        std::string execTime;
        pMsg->GetAt(ROMFIELDS::CSV_EXEC_TIME, execTime);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_TIME, execTime);

        std::string execCfm;
        pMsg->GetAt(ROMFIELDS::CSV_EXEC_CONFIRM, execCfm);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_CONFIRM, execCfm);

        std::string ref;
        pMsg->GetAt(ROMFIELDS::CSV_REFERENCE, ref);
        OatsMsg.SetAt(ROMFIELDS::CSV_REFERENCE, ref);

        std::string report;
        pMsg->GetAt(ROMFIELDS::CSV_REPORT, report);
        OatsMsg.SetAt(ROMFIELDS::CSV_REPORT, report);

        std::string clxTag;
        pMsg->GetAt(ROMFIELDS::CSV_CANCEL_TAG, clxTag);
        OatsMsg.SetAt(ROMFIELDS::CSV_CANCEL_TAG, clxTag);

        std::string contra;
        pMsg->GetAt(ROMFIELDS::CSV_CONTRA, contra);
        OatsMsg.SetAt(ROMFIELDS::CSV_CONTRA, contra);

        std::string effTime;
        pMsg->GetAt(ROMFIELDS::CSV_EFFECTIVE_TIME, effTime);
        OatsMsg.SetAt(ROMFIELDS::CSV_EFFECTIVE_TIME, effTime);

        std::string bilCode;
        pMsg->GetAt(ROMFIELDS::CSV_BILLING_CODE, bilCode);
        OatsMsg.SetAt(ROMFIELDS::CSV_BILLING_CODE, bilCode);

        std::string lastPrice;
        pMsg->GetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);

        if (status == ROMVALUE::STATUS_FILLED ||
            status == ROMVALUE::STATUS_PARTIALLYFILLED)
        {
            std::string execTag;
            pMsg->GetAt(ROMFIELDS::CSV_EXEC_TAG, execTag);
            OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_TAG, execTag);
        }

        if (status == ROMVALUE::STATUS_ROMREPLACEPENDING)
        {
            //need to override all the replaceable fields with m_ReplacingXXX
            if (CRomOrderNew::IsLimitType(pOrder->ReplacingType()))
            {
                if (pOrder->SecType() == ROMVALUE::SECTYPE_EQUITY)
                {
                    OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, pOrder->ReplacingPrice(), 4);
                }
                else
                {
                    OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, pOrder->ReplacingPrice(), 8);
                }
            }
            else
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, "");
            }

            OatsMsg.SetAt(ROMFIELDS::CSV_SHARES, pOrder->Leave() + pOrder->Cum() + pOrder->ShareChange());
            std::string reducedQty;
            if (pMsg->GetAt(ROMFIELDS::CSV_QUERY_TAG, reducedQty))
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_QUERY_TAG, reducedQty);
            }

            OatsMsg.SetAt(ROMFIELDS::CSV_TYPE, pOrder->ReplacingType());
            OatsMsg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, pOrder->ReplacingPegOffset());
            OatsMsg.SetAt(ROMFIELDS::CSV_STOPPRICE, pOrder->ReplacingStopPrice());
            OatsMsg.SetAt(ROMFIELDS::CSV_TIF, pOrder->ReplacingTIF());
            OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, pOrder->ReplacingExecInst());
            OatsMsg.SetAt(ROMFIELDS::CSV_FLOOR, pOrder->ReplacingMaxFloor());
            OatsMsg.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, pOrder->ReplacingInstructions());

            if (pOrder->ReplacingMinQty())
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_MINQTY, pOrder->ReplacingMinQty());
            }

            if (pOrder->ReplacingTIF() == ROMVALUE::TIF_GTD)
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_GTD_DATE, pOrder->ReplacingExpTime());
            }


        }
    }

    std::string omTime;
    pMsg->GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    OatsMsg.SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    std::string text;
    pMsg->GetAt(ROMFIELDS::CSV_TEXT, text);
    OatsMsg.SetAt(ROMFIELDS::CSV_TEXT, text);

    OatsMsg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, pOrder->PegOffset());
    OatsMsg.SetAt(ROMFIELDS::CSV_SEC_DEF, pOrder->SecDef());

    if (ROMVALUE::CPLXTYPE_LEG == cplxType &&
            strlen(pOrder->ParentRomTag()) == 0) //leg of produc type spread orders
    {
        std::string symbol;
        pMsg->GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
        OatsMsg.SetAt(ROMFIELDS::CSV_SYMBOL, symbol);

        std::string side;
        pMsg->GetAt(ROMFIELDS::CSV_SIDE, side);
        OatsMsg.SetAt(ROMFIELDS::CSV_SIDE, side);

        std::string secid;
        pMsg->GetAt(ROMFIELDS::CSV_SEC_ID, secid);
        OatsMsg.SetAt(ROMFIELDS::CSV_SEC_ID, secid);

        std::string secdef;
        if (pMsg->GetAt(ROMFIELDS::CSV_SEC_DEF, secdef))
        {
            OatsMsg.SetAt(ROMFIELDS::CSV_SEC_DEF, secdef);
        }

        std::string expym;
        pMsg->GetAt(ROMFIELDS::CSV_EXPYEARMONTH, expym);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, expym);

        short cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
        if (pMsg->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype) == false)
        {
            cplxtype = pOrder->CplxType();
        }
        
        if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD && 
            ROMVALUE::CPLXTYPE_LEG == cplxtype)
        {
            char sectype = ROMVALUE::SECTYPE_EQUITY;
            if (pMsg->GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype) == false)
            {
                sectype = pOrder->SecType();
            }
            OatsMsg.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype);

            if (CUtility::IsOptionType(sectype))
            {
                char callput = 'C';
                pMsg->GetAt(ROMFIELDS::CSV_CALLPUT, callput);
                OatsMsg.SetAt(ROMFIELDS::CSV_CALLPUT, callput);

                std::string strike;
                pMsg->GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);
                OatsMsg.SetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);

                std::string maturityday;
                pMsg->GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            }
            else if (CUtility::IsFutureType(sectype))
            {
                std::string maturityday;
                pMsg->GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            }
        }
        else
        {
            if (CUtility::IsOptionType(pOrder->SecType()))
            {
                //callput and strike are already populated from GetCsv call

                //cover up the store proc ROM_selOrdersNew bug that
                //expireDate is not retrieved as string and therefor md can not be
                //generated from it
                if (strlen(pOrder->MaturityDay()) == 0 &&
                    pMsg->IsFieldEmpty(ROMFIELDS::CSV_MATURITYDAY) == false)
                {
                    std::string maturityday;
                    pMsg->GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                    OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                }
            }
        }
        
        /*if (CUtility::IsOptionType(sectype))
        {
            char callput = 'C';
            pMsg->GetAt(ROMFIELDS::CSV_CALLPUT, callput);
            OatsMsg.SetAt(ROMFIELDS::CSV_CALLPUT, callput);

            std::string strike;
            pMsg->GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);
            OatsMsg.SetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);

            std::string maturityday;
            pMsg->GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
        }*/

        //std::string avgPrice;
        //pMsg->GetAt(ROMFIELDS::CSV_AVG_PRICE, avgPrice);
        //OatsMsg.SetAt(ROMFIELDS::CSV_AVG_PRICE, avgPrice);

        OatsMsg.SetAt(ROMFIELDS::CSV_STATUS, status);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);

        std::string leaveShares;
        pMsg->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leaveShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, leaveShares);

        std::string cumShares;
        pMsg->GetAt(ROMFIELDS::CSV_CUM_SHARES, cumShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_CUM_SHARES, cumShares);
    }

    if (CRomOrderNew::IsLimitType(type) == false)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, "");
    }
    else
    {
        std::string sprice;
        if (pMsg->GetAt(ROMFIELDS::CSV_PRICE, sprice))
        {
            OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, sprice);
        }
    }
    OatsMsg.GetMsgNew(pBuffer);
}

void COats::ProcessAdminText(CCsv& msg)
{
    std::string traders;
    msg.GetAt(ROMFIELDS::CSV_TRADER, traders);
    std::string message;
    msg.GetMsg(message);

    if (traders.empty())
    {
        CClientsManager::getInstance().BroadCast(message);
    }
    else
    {
        std::string trader(""), err;
        std::string::size_type start = 0, newstart = 0;

        newstart = traders.find_first_of('|', start);
        if (std::string::npos == newstart) //single target
        {
            CClientsManager::getInstance().Send(traders, msg, err);
            return;
        }

        while (std::string::npos != newstart) // multiple target
        {
            trader = traders.substr(start, newstart - start);
            CClientsManager::getInstance().Send(trader, msg, err);
            start = newstart + 1;
            newstart = traders.find_first_of('|', start);

            if (std::string::npos == newstart)
            {
                trader = traders.substr(start, traders.length());
                CClientsManager::getInstance().Send(trader, msg, err);
            }
        }

    }
}

bool COats::ProcessAdminCancel(CCsv* pMsg)
{
    return CThreadRomOrdersNew::getInstance().ProcessClientCancel(pMsg);
}

void COats::ProcessAdminCancels(CCsv* pMsg)
{
    tDuty duty = {NULL, NULL, true};
    pMsg->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 0);
    char omTime[32];
    CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
    pMsg->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    duty.theFunc = CThreadRomOrdersNew::getInstance().ProcessAdminCANCELS;
    duty.pArg = pMsg;
    CHeavyDuty::getInstance().AddMessage(duty);
}

bool COats::ProcessAdminStatus(CCsv* pMsg)
{
    //COrderManager::getInstance().AddHostUpdate(const_cast<char*>(pMsg->Message().c_str()), pMsg->Message().size());
    //COrderManager::getInstance().AddHostUpdate(pMsg);
    return CThreadRomOrdersNew::getInstance().ProcessHostUpdate(pMsg);
}

bool COats::ProcessAdminZap(CCsv* pMsg)
{
    //COrderManager::getInstance().AddClientRequest(pMsg);
    //CThreadOrderHandler::getInstance().ProcessAdminZap(pMsg);
    return CThreadRomOrdersNew::getInstance().ProcessHostUpdate(pMsg);
}

void COats::Send(const std::string& msg)
{
    if (m_pCon->Connected())
    {
        if (m_pCon->SendCmd(msg) == false)
        {
            //log send failed
        }
    }
    else
    {
        //log not connected yet
        //bool hit = true;
    }
}

void COats::CleanQueryResult()
{
    std::set<databuf_t*>::iterator it = m_queryResult.begin();
    databuf_t* pBuf = NULL;

    while (it != m_queryResult.end())
    {
        pBuf = *it;
        CBuffStore::getInstance().ReturnB(pBuf);
        it++;
    }

    m_queryResult.clear();
}

void COats::ReloadIpPort()
{
    if (m_pCon)
    {
        std::string ip;
        int port = 0;
        CConfig::getInstance().GetDestIpPort("Oats", ip, port);
        m_pCon->SetIP(ip);
        m_pCon->SetPORT(port);
    }
}

void COats::ProcessAdminQueryBillingGrpLimits(CCsv& msg)
{
    int billinggrpid;
    char sectype;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, billinggrpid);
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    theLimits limits;
    std::string message;

    if (CClientProfileManagerNew::getInstance().GetBillingGrpLimits(billinggrpid, limits))
    {
        if (limits.exempted)
        {
            char text[128];
            sprintf(text, "BillingGroup %d's Limits Check is exempted", billinggrpid);
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        }
        
        if (msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype))
        {
            int secindex = CRomOrderNew::GetSecIndexFromSecType(sectype);
            msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, limits.limits[secindex].MaxOrders);
            msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, limits.limits[secindex].NumOrders);
            msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, limits.limits[secindex].MaxTotalShares);
            msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, limits.limits[secindex].NumShares);
            msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, limits.limits[secindex].BuyingPower, 2);
            char balanceStr[128];
            sprintf(balanceStr, "%f|%f", limits.limits[secindex].Balance[ROMVALUE::BUY], limits.limits[secindex].Balance[ROMVALUE::SELL]);
            msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
            msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, limits.limits[secindex].MaxShares);
            msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, limits.limits[secindex].MaxDollars, 2);
            msg.GetMsg(message);
            m_pCon->SendCmd(message);
        }
        else
        {    
            char sectype[5];
            strcpy(sectype, "EOMF");

            for (int i = 0; i < ROMVALUE::SECNUM; i++)
            {
                msg.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype[i]);
                msg.SetAt(ROMFIELDS::CSV_MAX_ORDERS, limits.limits[i].MaxOrders);
                msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENORDERS, limits.limits[i].NumOrders);
                msg.SetAt(ROMFIELDS::CSV_MAX_TOTALSHARES, limits.limits[i].MaxTotalShares);
                msg.SetAt(ROMFIELDS::CSV_CURRENT_OPENSHARES, limits.limits[i].NumShares);
                msg.SetAt(ROMFIELDS::CSV_MAX_TOTALDOLLARS, limits.limits[i].BuyingPower, 2);
                char balanceStr[128];
                sprintf(balanceStr, "%f|%f", limits.limits[i].Balance[ROMVALUE::BUY], limits.limits[i].Balance[ROMVALUE::SELL]);
                msg.SetAt(ROMFIELDS::CSV_CURRENT_DOLLARS, balanceStr);
                msg.SetAt(ROMFIELDS::CSV_MAX_SHARESPERORDER, limits.limits[i].MaxShares);
                msg.SetAt(ROMFIELDS::CSV_MAX_DOLLARSPERORDER, limits.limits[i].MaxDollars, 2);
                msg.GetMsg(message);
                m_pCon->SendCmd(message);
            }
        }
    }
    else
    {
        char value[200];
        sprintf(value, "%s:Limits is not set for Billing Group: %d", (CConfig::getInstance().Login()).c_str(), billinggrpid);
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);
    }
}

void COats::ProcessAdminReloadBillingGrpLimits(CCsv& msg)
{
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    char text[512];

    tDuty duty = {NULL, NULL, false};
    duty.theFunc = CClientProfileManagerNew::ReloadBillingGrpLimits;
    int* billinggrpid = new int;
    std::string message;

    if (msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, *billinggrpid))
    {
        duty.pArg = billinggrpid;
        sprintf(text, "%s:Loading Billing Group Limits(id=%d)...",
                CConfig::getInstance().Login().c_str(),
                *billinggrpid);

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading Billing Group Limits(id=%d)",
                CConfig::getInstance().Login().c_str(),
                *billinggrpid);

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);
    }
    else
    {
        sprintf(text, "%s:Loading Billing Group Limits...",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        sprintf(text, "%s:...Done Loading All Billing Group Limits",
                CConfig::getInstance().Login().c_str());

        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        msg.GetMsg(message);
        m_pCon->SendCmd(message);

        duty.pArg = NULL;
        delete billinggrpid;
    }

    CHeavyDuty::getInstance().AddMessage(duty);
}

void COats::ProcessAdminReloadLenderLimits(CCsv& msg)
{
    CPositionManager::getInstance().LoadlendingLimit();
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string grps;
    //CPositionManager::getInstance().GetLendingGroups(grps);
    //msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP, grps.c_str());
    msg.SetAt(ROMFIELDS::CSV_TEXT, "Lending limit has been reloaded");
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
    
    //msg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::GET_LENDER_GROUPS);
    //ProcessAdminGetLendingGroupsSymbols(msg);
}

void COats::ProcessAdminReloadEasyBorrowList(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string err;
    
    if (CPositionManager::getInstance().LoadEasyBorrowList_today(err))
    {    
        msg.SetAt(ROMFIELDS::CSV_TEXT, "Easy to Borrow list has been reloaded");
    }
    else
    {
        
    }
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminReloadInitPosition(CCsv& msg)
{
    CPositionManager::getInstance().ReloadPositionFromRDS();
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_TEXT, "Reloading Position from RDS for Ronin Trading...");
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminQueryPosition(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string symbol, clracct;
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    msg.GetAt(ROMFIELDS::CSV_POS_CLRACCT, clracct);
    
    tPosition pos = {0, 0};
    if (CPositionManager::getInstance().GetPos(symbol, clracct, pos))
    {
        msg.SetAt(ROMFIELDS::CSV_POS_POS, pos.cur_pos);
        msg.SetAt(ROMFIELDS::CSV_POS_INIT_POS, pos.ini_pos);
    }
    else
    {
        char text[1024];
        if (clracct.empty())
        {
            sprintf(text, "Position for symbol<%s> does not exist", symbol.c_str());
        }
        else
        {
            sprintf(text, "Position for clearing account<%s>/symbol<%s> does not exist", 
                    clracct.c_str(), symbol.c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    }
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminUpdatePosition(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    tPosUpdate posUpdate;
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, posUpdate.symbol);
    msg.GetAt(ROMFIELDS::CSV_POS_CLRACCT, posUpdate.clract);
    msg.GetAt(ROMFIELDS::CSV_POS_POSCHANGE, posUpdate.qty_change);
    posUpdate.side = ROMVALUE::SIDE_BUY;
    CPositionManager::getInstance().UpdatePositionOnly(&posUpdate);
    char text[256];
    sprintf(text, "%ld shares has been added to the position of Clearing Account<%s>/Symbol<%s>", 
            posUpdate.qty_change, posUpdate.clract.c_str(), posUpdate.symbol.c_str());
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminQueryEasySymbol(CCsv& msg)
{
    std::string symbol;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    
    char text[256];
    if (CPositionManager::getInstance().FoundEasySymbol(symbol))
    {
        sprintf(text, "Symbol<%s> is in EasyToBorrow list", symbol.c_str());
    }
    else
    {
        sprintf(text, "Symbol<%s> is not in EasyToBorrow list", symbol.c_str());
    }
    
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminClrLendingLimitQuery(CCsv& msg)
{
    std::string clr, symbol;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    msg.GetAt(ROMFIELDS::CSV_LENDING_CLR, clr);

    tLendingLimit limit;
        
    if (CPositionManager::getInstance().GetClrLenderLimits(clr, symbol, limit))
    {
        msg.SetAt(ROMFIELDS::CSV_LENDING_CLR_LIMIT, limit.limit);
        msg.SetAt(ROMFIELDS::CSV_LENDING_CLR_BALANCE, limit.balance);
        msg.SetAt(ROMFIELDS::CSV_TEXT, clr);
    }
    else
    {
        char text[256];
        sprintf(text, "Clearing Account<%s> does not have locate for Symbol<%s>",
                clr.c_str(), symbol.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    }

    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminQueryLenderLimits(CCsv& msg)
{
    std::string group, symbol;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    msg.GetAt(ROMFIELDS::CSV_LENDINGGROUP, group);
   
    if (symbol.empty() == false)
    {
        tLendingLimit limit;
        std::string clrs;

        if (CPositionManager::getInstance().GetLenderGroupLimits(group, symbol, clrs, limit))
        {
            msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP_LIMIT, limit.limit);
            msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP_BALANCE, limit.balance);
            msg.SetAt(ROMFIELDS::CSV_TEXT, clrs);
        }
        else
        {
            char text[256];
            sprintf(text, "Lending Group<%s> does not have locate for Symbol<%s>",
                    group.c_str(), symbol.c_str());
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
        }

        std::string sMsg;
        msg.GetMsg(sMsg);
        Send(sMsg.c_str(), sMsg.size());
    }
    else
    {
        std::vector<tLendingLimitDetail> details;
        if (CPositionManager::getInstance().GetLenderGroupAllLimits(group, details))
        {
            for (size_t i = 0; i < details.size(); i++)
            {
                msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP_LIMIT, details[i].limit.limit);
                msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP_BALANCE, details[i].limit.balance);
                msg.SetAt(ROMFIELDS::CSV_TEXT, details[i].clrs);
                msg.SetAt(ROMFIELDS::CSV_POS_SYMBOL, details[i].symbol);
                std::string sMsg;
                msg.GetMsg(sMsg);
                Send(sMsg.c_str(), sMsg.size());
            }
        }
        else
        {
            char text[256];
            sprintf(text, "Lending Group<%s> does not have locate",
                    group.c_str());
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
            std::string sMsg;
            msg.GetMsg(sMsg);
            Send(sMsg.c_str(), sMsg.size());
        }
    }
}

void COats::ProcessAdminAddEasySymbol(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string symbol;
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_EASYBORROW_ACTION, action);
    
    char text[256];
    if (action)
    {
        if (CPositionManager::getInstance().AddEasySymbol(symbol, true))
        {
            sprintf(text, "Symbol<%s> has been added to EasyToBorrow list", symbol.c_str());
        }
        else
        {
            sprintf(text, "Symbol<%s> was already in EasyToBorrow list", symbol.c_str());
        }
    }
    else
    {
        if (CPositionManager::getInstance().AddEasySymbol(symbol, false))
        {
            sprintf(text, "Symbol<%s> has been removed from EasyToBorrow list", symbol.c_str());
        }
        else
        {
            sprintf(text, "Symbol<%s> was not in EasyToBorrow list", symbol.c_str());
        }
    }
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}
/*
void COats::ProcessAdminGetLendingGroups(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string lending_groups;
    CPositionManager::getInstance().GetLendingGroups(lending_groups);
    msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP, lending_groups.c_str());
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}
*/
/*void COats::ProcessAdminGetLendingGroupsSymbols(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::vector<tGrpSymbols> vgrpsymbols;
    std::string stuf;
    
    CPositionManager::getInstance().GetGrpSymbols(vgrpsymbols);
    
    for (unsigned int i = 0; i < vgrpsymbols.size(); i++)
    {
        stuf.append(vgrpsymbols[i].grp);
        
        stuf.append("=");
        for (unsigned int j = 0; j < vgrpsymbols[i].symbols.size(); j++)
        {
            stuf.append(vgrpsymbols[i].symbols[j]);
            if (j != vgrpsymbols[i].symbols.size() - 1)
            {
                stuf.append(":");
            }
        }
                
        if (i != vgrpsymbols.size() - 1)
        {
            stuf.append("|");
        }
    }
    
    msg.SetAt(ROMFIELDS::CSV_LENDINGGROUP, stuf.c_str());
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}*/


void COats::ProcessAdminUpdateLenderLimit(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    
    std::string grp, symbol, clrs;
    long newlimit = 0;
    
    msg.GetAt(ROMFIELDS::CSV_LENDING_CLR, grp);
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    msg.GetAt(ROMFIELDS::CSV_LENDING_CLR_LIMIT, newlimit);
    msg.GetAt(ROMFIELDS::CSV_CLRSTOGRP, clrs);
    //char text[256];
    std::string echo;
    
    if (clrs.empty())
    {
        CPositionManager::getInstance().UpdateLendingGroupLimit(grp, symbol,
                newlimit, echo);
    }
    else
    {
        CPositionManager::getInstance().AddLendingGroupLimit(grp, symbol, clrs,
                newlimit, echo);
    }
       
    msg.SetAt(ROMFIELDS::CSV_TEXT, echo);
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminReloadThresholdList(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string err;
    if (CPositionManager::getInstance().LoadThresholdList_current(err))
    {
        msg.SetAt(ROMFIELDS::CSV_TEXT, "Threshold list has been reloaded");
    }
    else
    {
        msg.SetAt(ROMFIELDS::CSV_TEXT, "reloading Threshold list was failed");
    }
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}
   
void COats::ProcessAdminQueryThresholdSymbol(CCsv& msg)
{
    std::string symbol;
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    
    char text[256];
    if (CPositionManager::getInstance().FoundThresholdSymbol(symbol))
    {
        sprintf(text, "Symbol<%s> is in Threshold list", symbol.c_str());
    }
    else
    {
        sprintf(text, "Symbol<%s> is not in Threshold list", symbol.c_str());
    }
    
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminExemptClrActLimits(CCsv& msg)
{
    std::string key;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, key);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientProfileManagerNew::getInstance().ExemptClr(key, action))
    {
        if (action == 1)
        {
            sprintf(value, "%s:ClearingAccount %s's Limits Check has been exempted", (CConfig::getInstance().Login()).c_str(), key.c_str());
        }
        else
        {
            sprintf(value, "%s:ClearingAccount %s's Limits Check has been enforced", (CConfig::getInstance().Login()).c_str(), key.c_str());
        }
    }
    else
    {
        sprintf(value, "%s:ClearingAccount %s Not Found", (CConfig::getInstance().Login()).c_str(), key.c_str());
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminExemptBgpLimits(CCsv& msg)
{
    std::string key;
    msg.GetAt(ROMFIELDS::CSV_TRADER_FREAK, key);
    std::string owner;
    msg.GetAt(ROMFIELDS::CSV_OWNER, owner);

    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_ACTION_FREAK, action);

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

    char value[100];

    if (CClientProfileManagerNew::getInstance().ExemptBgrp(key, action))
    {
        if (action == 1)
        {
            sprintf(value, "%s:BillingGroup %s's Limits Check has been exempted", (CConfig::getInstance().Login()).c_str(), key.c_str());
        }
        else
        {
            sprintf(value, "%s:BillingGroup %s's Limits Check has been enforced", (CConfig::getInstance().Login()).c_str(), key.c_str());
        }
    }
    else
    {
        sprintf(value, "%s:BillingGroup %s Not Found", (CConfig::getInstance().Login()).c_str(), key.c_str());
    }

    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
    msg.SetAt(ROMFIELDS::CSV_TEXT, value);
    std::string message;
    msg.GetMsg(message);
    m_pCon->SendCmd(message);
}

void COats::ProcessAdminAddThresholdSymbol(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string symbol;
    msg.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    int action = 0;
    msg.GetAt(ROMFIELDS::CSV_EASYBORROW_ACTION, action);
    
    char text[256];
    if (action)
    {
        if (CPositionManager::getInstance().AddThresholdSymbol(symbol, true))
        {
            sprintf(text, "Symbol<%s> has been added to Threshold list", symbol.c_str());
        }
        else
        {
            sprintf(text, "Symbol<%s> was already in Threshold list", symbol.c_str());
        }
    }
    else
    {
        if (CPositionManager::getInstance().AddThresholdSymbol(symbol, false))
        {
            sprintf(text, "Symbol<%s> has been removed from Threshold list", symbol.c_str());
        }
        else
        {
            sprintf(text, "Symbol<%s> was not in Threshold list", symbol.c_str());
        }
    }
    msg.SetAt(ROMFIELDS::CSV_TEXT, text);
    
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}


void COats::ProcessAdminReloadFuture(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    CThreadRomOrdersNew::getInstance().ReloadFutures();
    msg.SetAt(ROMFIELDS::CSV_TEXT, "Futures have been reloaded");
    std::string sMsg;
    msg.GetMsg(sMsg);
    Send(sMsg.c_str(), sMsg.size());
}

void COats::ProcessAdminQueryFuture(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    std::string product;
    std::deque<std::string> buffer;
    msg.GetAt(ROMFIELDS::CSV_SYMBOL, product);
   
    CThreadRomOrdersNew::getInstance().GetFuture(product, buffer);
    std::deque<std::string>::iterator it = buffer.begin();
    while (it != buffer.end())
    {
        msg.SetAt(ROMFIELDS::CSV_TEXT, *it);
        std::string sMsg;
        msg.GetMsg(sMsg);
        Send(sMsg.c_str(), sMsg.size());
        it++;
    }
    //msg.SetAt(ROMFIELDS::CSV_TEXT, buffer);
    //std::string sMsg;
    //msg.GetMsg(sMsg);
    //Send(sMsg.c_str(), sMsg.size());
}