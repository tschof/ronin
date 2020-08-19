#include "DestsManager.h"
#include "Config.h"
//#include "ThreadOrderHandler.h"
#include "Oats.h"
#include "ThreadRomOrdersNew.h"
#include "client_manager.h"
#include "rom_handler.h"
#include "TimerServer.h"
#include "RomCancel.h"
//#include "CsvStore.h"
#include "debug_logger.h"
#include <algorithm>

#include "message_queue.h"
#include "config_loader.h"
#include "BuffStore.h"
#include "ClientsManager.h"
#include "HeavyDuty.h"
#include <sys/time.h>


static bool l_live = false;

CDestsManager::CDestsManager() :
    m_ConMap(true),
    m_UnConMap(true),
    m_DestIdToRouteId(true)
    //m_garbTimers(true)
{
    l_live = true;
    pthread_rwlock_init(&m_lock, NULL);
    pthread_mutex_init(&m_blocklock, NULL);
    pthread_mutex_init(&m_unblocklock, NULL);
   
    LoadDestConInfo();
    LoadDestsFromDB(false);
}

CDestsManager::~CDestsManager()
{
    l_live = false;
    CHostConnection* pCon = NULL;
    while ((pCon = (CHostConnection*)m_UnConMap.RemoveNext()) != NULL)
    {
        delete pCon;
    }

    while ((pCon = (CHostConnection*)m_ConMap.RemoveNext()) != NULL)
    {
        delete pCon;
    }

    CleaningTimer();

    std::list<CRomTimer*> timerList;
    m_blockingTimerMap.GetAll(timerList);
    m_unblockingTimerMap.GetAll(timerList);
    std::list<CRomTimer*>::iterator it = timerList.begin();
    CRomTimer* pTimer = NULL;

    while (it != timerList.end())
    {
        pTimer = *it;
        delete pTimer;
        it++;
    }

    pthread_rwlock_destroy(&m_lock);
    pthread_mutex_destroy(&m_blocklock);
    pthread_mutex_destroy(&m_unblocklock);
}

void CDestsManager::CleaningTimer()
{
    std::set<CRomTimer*> garbTimers;
    m_garbTimers.RemoveAll(garbTimers);
    std::set<CRomTimer*>::iterator it = garbTimers.begin();
    CRomTimer* pTimer = NULL;

    while (it != garbTimers.end())
    {
        pTimer = *it;
        delete pTimer;
        it++;
    }
}

/*void CDestsManager::RemoveBlockingTimer(const std::string& key)
{
    pthread_mutex_lock(&m_blocklock);

    CRomTimer* pTimer = NULL;

    if (m_blockingTimerMap.Remove(key, pTimer))
    {
        m_garbTimers.Add(pTimer);
    }

    pthread_mutex_unlock(&m_blocklock);
}*/

void CDestsManager::AddBlockingTimer(const std::string& key, CRomTimer* pTimer)
{
    pthread_mutex_lock(&m_blocklock);
    m_blockingTimerMap.Add(key, pTimer);
    pthread_mutex_unlock(&m_blocklock);
}

void CDestsManager::AddUnBlockingTimer(const std::string& key, CRomTimer* pTimer)
{
    pthread_mutex_lock(&m_unblocklock);
    m_unblockingTimerMap.Add(key, pTimer);
    pthread_mutex_unlock(&m_unblocklock);
}

/*void CDestsManager::RemoveUnBlockingTimer(const std::string& key)
{
    pthread_mutex_lock(&m_unblocklock);

    CRomTimer* pTimer = NULL;

    if (m_unblockingTimerMap.Remove(key, pTimer))
    {
        m_garbTimers.Add(pTimer);
    }

    pthread_mutex_unlock(&m_unblocklock);
}*/

bool CDestsManager::AddBlockingTimerArg(const std::string& key, void* pObj)
{
    CRomTimer* pTimer = NULL;
    bool rc = false;

    pthread_mutex_lock(&m_blocklock);

    if (m_blockingTimerMap.Get(key, pTimer))
    {
        rc = pTimer->AddArg(pObj);
    }

    pthread_mutex_unlock(&m_blocklock);

    return rc;
}

bool CDestsManager::RemoveBlockingTimerArg(const std::string& key, void* pObj)
{
    CRomTimer* pTimer = NULL;
    bool rc = false;

    pthread_mutex_lock(&m_blocklock);

    if (m_blockingTimerMap.Get(key, pTimer))
    {
        rc = pTimer->RemoveArg(pObj);
    }

    pthread_mutex_unlock(&m_blocklock);

    return rc;
}

bool CDestsManager::AddUnBlockingTimerArg(const std::string& key, void* pObj)
{
    CRomTimer* pTimer = NULL;
    bool rc = false;

    pthread_mutex_lock(&m_unblocklock);

    if (m_unblockingTimerMap.Get(key, pTimer))
    {
        rc = pTimer->AddArg(pObj);
    }

    pthread_mutex_unlock(&m_unblocklock);

    return rc;
}

bool CDestsManager::RemoveUnBlockingTimerArg(const std::string& key, void* pObj)
{
    CRomTimer* pTimer = NULL;
    bool rc = false;

    pthread_mutex_lock(&m_unblocklock);

    if (m_unblockingTimerMap.Get(key, pTimer))
    {
        rc = pTimer->RemoveArg(pObj);
    }

    pthread_mutex_unlock(&m_unblocklock);

    return rc;
}

/*void CDestsManager::LoadRlogonDestIDs()
{
    const std::string& romInstance = CConfig::getInstance().InstanceName();
    const tDBLogin& db = CConfig::getInstance().db();

    CUtility::LoadNoneFirstServerLogons(db, m_relogonDestIDs, romInstance);
}*/

void CDestsManager::LoadDestConInfo()
{
    const std::string& romInstance = CConfig::getInstance().InstanceName();
    //const tDBLogin& db = CConfig::getInstance().db();
    CDBExecutor* pExecutor = CConfig::getInstance().dbexecutor();
    CUtility::LoadServerConInfo(m_DestConInfo, romInstance, *pExecutor);
}

/*void CDestsManager::DestroyRlogonDestIDs()
{
    const std::string& romInstance = CConfig::getInstance().InstanceName();
    const tDBLogin& db = CConfig::getInstance().db();
    CDBExecutor* pExecutor = CConfig::getInstance().dbexecutor();
    CUtility::DeleteServerLastLogons(db, romInstance, *pExecutor);
}*/

bool CDestsManager::GetDestStatus(const std::string& routeID, ROMTYPEDEF::tConnectionStatus& conStatus)
{
    bool rc = false;
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(routeID, pCon) == false)
    {
        m_UnConMap.Get(routeID, pCon);
    }

    if (pCon)
    {
        conStatus.connected = pCon->Connected();
        conStatus.loggedon = pCon->LoggedOn();
        conStatus.blocked = pCon->Blocked();

        conStatus.description = pCon->Description();
        rc = true;
    }

    return rc;
}

bool CDestsManager::GetDestNameFromDestId(const std::string& destID, std::string& destName)
{
    bool rc = false;
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(destID, pCon) == false)
    {
        m_UnConMap.Get(destID, pCon);
    }

    if (pCon)
    {
        destName = pCon->Description();
        rc = true;
    }

    return rc;
}

bool CDestsManager::GetDestIdFromDestName(const std::string& destName, std::string& destID)
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_DestNameToDestId.Get(destName, destID);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CDestsManager::GetRouteIdFromDestId(const std::string& destID, std::string& routeID)
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_DestIdToRouteId.Get(destID, routeID);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CDestsManager::UpdateDestRoute(const std::string& destID, const std::string& newRouteID)
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_RouteIdToSecType.Find(newRouteID);
    if (rc)
    {
        m_DestIdToRouteId.Update(destID, newRouteID);
    }
    pthread_rwlock_unlock(&m_lock);
    return rc;
}


bool CDestsManager::GetSecTypeFromRouteId(const std::string& routeID, char& secType)
{
    bool rc = false;
    pthread_rwlock_rdlock(&m_lock);
    if (m_RouteIdToSecType.Get(routeID, secType))
    {
        rc = true;
    }
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CDestsManager::ProcessBlock(const std::string& id, bool action)
{
    CHostConnection* pCon = NULL;
    if (id.compare("-1") != 0)
    {
        std::string rid;
        m_DestIdToRouteId.Get(id, rid);//handling logical dest block/unblock from DB driven OATS!
        
        if (m_ConMap.Get(rid, pCon))
        {
            pCon->SetBlocked(action);
        }
        else if (m_UnConMap.Get(rid, pCon))
        {
            pCon->SetBlocked(action);
        }
        if (pCon)
        {
            COats::getInstance().HostConnectionUpdate(*pCon, "");
        }
    }
    else //block all
    {
        std::list<CHostConnection*> conList;
        m_ConMap.GetAll(conList);
        std::list<CHostConnection*>::iterator it = conList.begin();
        while (it != conList.end())
        {
            pCon = *it;
            pCon->SetBlocked(action);
            COats::getInstance().HostConnectionUpdate(*pCon, "");
            it++;
        }

        conList.clear();
        m_UnConMap.GetAll(conList);
        std::list<CHostConnection*>::iterator itt = conList.begin();
        while (itt != conList.end())
        {
            pCon = *itt;
            pCon->SetBlocked(action);
            COats::getInstance().HostConnectionUpdate(*pCon, "");
            itt++;
        }
    }
}

void CDestsManager::ProcessDisconnected(const std::string& id)
{
    CHostConnection* pCon = NULL;
    if (m_ConMap.Remove(id, pCon) ||
        m_UnConMap.Remove(id, pCon))
    {
        //unsigned short now = CUtility::GetLocalTimeHHMM();
        //unsigned short now = CUtility::GetCentralTimeHHMM();
        //if (pCon->ShouldDestUp(now))
        if (pCon->ShouldDestUp())
        {
            if (pCon->LoggedOn())
            {
                //disconnected from dest
                std::stringstream stm;
                stm << CConfig::getInstance().InstanceName()
                    << " disconnected from "
                    << pCon->Description();
                std::string msg = stm.str();
                CUtility::SendBizEMail(msg, msg);
            }
            else if (pCon->connectingCnt() == 1)
            {
                //can not connect to dest
                std::stringstream stm;
                stm << CConfig::getInstance().InstanceName()
                    << " can not connect to "
                    << pCon->Description();
                std::string msg = stm.str();
                CUtility::SendBizEMail(msg, msg);
            }
        }

        pCon->SetConnected(false);  //It is crucial to call SetConnected before SetConnecting
        pCon->SetConnecting(false); //to avoid overreconnect
        pCon->SetLoggedOn(false);
        m_UnConMap.Add(id, pCon);
        COats::getInstance().HostConnectionUpdate(*pCon, "");

        struct message msg;
        char buf[128];
        sprintf(buf, "Disconnected from %s(%s:%d)", pCon->Description().c_str(), pCon->IP().c_str(), pCon->Port());
        msg.data = buf;
        msg.len = strlen(buf);
        msg.name = const_cast<char*>(pCon->Description().c_str());
        msg.name_len = pCon->Description().length();
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());
    }
}

void CDestsManager::ProcessQueryDest(const std::string& id)
{
    CHostConnection* pC = NULL;

    if (m_ConMap.Get(id, pC) == false)
    {
        m_UnConMap.Get(id, pC);
    }

    CCsv msg;

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
    msg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());

    std::string msgs, MSG;

    if (!pC)
    {
        char value[100];
        sprintf(value, "%s:Invalid OrderDest:%s", (CConfig::getInstance().Login()).c_str(), id.c_str());
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(msgs);
    }
    else
    {
        char value[100];
        if (pC->Connected() == true)
        {
            sprintf(value, "%s:%s is Connected To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        else
        {
            sprintf(value, "%s:%s is Not Connected To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(msgs);

        if (pC->LoggedOn() == true)
        {
            sprintf(value, "%s:%s is Logged On To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        else
        {
            sprintf(value, "%s:%s is Not Logged On To %s Exchange Server", (CConfig::getInstance().Login()).c_str(), (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(MSG);
        msgs += MSG;

        if (pC->Blocked() == false)
        {
            sprintf(value, "%s:Users are Allowd To Place Orders Into %s", (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        else
        {
            sprintf(value, "%s:Users are Not Allowd To Place Orders Into %s", (CConfig::getInstance().Login()).c_str(), pC->Description().c_str());
        }
        msg.SetAt(ROMFIELDS::CSV_TEXT, value);
        msg.GetMsg(MSG);
        msgs += MSG;
    }

    COatsMsg* pMsg = new COatsMsg(msgs);
    COats::getInstance().AddMessage(pMsg);
}

void CDestsManager::ProcessQueryDests()
{
}


void CDestsManager::Disconnect(const std::string& name)
{
    CHostConnection* pCon = NULL;
    std::string ID;
    GetDestIdFromDestName(name, ID);
    if (m_ConMap.Get(ID, pCon))
    {
        pCon->kickoff_connection(const_cast<char*>(name.c_str()), name.size());
        Disconnected(ID);
    }
}

void CDestsManager::Disconnected(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::DISCONNECTED, id);
    AddMessage(pReq);
}

void CDestsManager::Connected(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::CONNECTED, id);
    AddMessage(pReq);
}

void CDestsManager::ReloadIpPort(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::RELOADIPPORT, id);
    AddMessage(pReq);
}

void CDestsManager::ProcessConnected(const std::string& id)
{
    CHostConnection* pCon = NULL;
    if (m_UnConMap.Remove(id, pCon))
    {
        pCon->SetConnected(true);
        pCon->SetConnecting(false);
        m_ConMap.Add(id, pCon);
        COats::getInstance().HostConnectionUpdate(*pCon, "");

        struct message msg;
        char buf[128];
        sprintf(buf, "Connected to %s(%s:%d)", pCon->Description().c_str(), pCon->IP().c_str(), pCon->Port());
        msg.data = buf;
        msg.len = strlen(buf);
        msg.name = const_cast<char*>(pCon->Description().c_str());
        msg.name_len = pCon->Description().length();
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());
    }
}

void CDestsManager::LoggedOn(const std::string& id)
{
    time_t logonT = 0;
    time(&logonT);
    ProcessLoggedOn(id, logonT);
}


void CDestsManager::ProcessLoggedOn(const std::string& id, time_t lastLogonTime)
{
    CHostConnection* pCon = NULL;
    if (m_ConMap.Get(id, pCon))
    {
        pCon->SetLoggedOn(true);
        pCon->SetRelogon(true);
        pCon->SetLastLogonTime(lastLogonTime);
        COats::getInstance().HostConnectionUpdate(*pCon, "");

        struct message msg;
        char buf[128];
        sprintf(buf, "LoggedOn to %s(%s:%d)", pCon->Description().c_str(), pCon->IP().c_str(), pCon->Port());
        msg.data = buf;
        msg.len = strlen(buf);
        msg.name = const_cast<char*>(pCon->Description().c_str());
        msg.name_len = pCon->Description().length();
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());
    }
}

void CDestsManager::Block(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::BLOCK, id);
    AddMessage(pReq);
}

void CDestsManager::UnBlock(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::UNBLOCK, id);
    AddMessage(pReq);
}

void CDestsManager::QueryDest(const std::string& id)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::QUERYDEST, id);
    AddMessage(pReq);
}

void CDestsManager::QueryDests()
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::QUERYDESTS, "");
    AddMessage(pReq);
}


//Sending will be done throught different threads
bool CDestsManager::Send(const std::string& id, CCsv& msg, std::string& err)
{
    bool rc = false;
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(id, pCon))
    {
        char cmd = ROMVALUE::COMMAND_ORDER;
        msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd);
        if (cmd == ROMVALUE::COMMAND_ORDER &&
                pCon->Blocked())
        {
            err = "Dest has been blocked";
        }
        else
        {
            databuf_t* pBuffer = CBuffStore::getInstance().Get();
            msg.GetMsgNew(pBuffer);

            if (pCon->Send(pBuffer->buffer, databuf_unread(pBuffer), true) == false)
            {
                err = "Can not send to dest";
            }
            else
            {
                rc = true;
            }

            CBuffStore::getInstance().ReturnB(pBuffer);
        }
    }
    else if (m_UnConMap.Get(id, pCon))
    {
        err = "Not connected to dest";
    }
    else
    {
        err = "Invalid dest";
    }

    return rc;
}

bool CDestsManager::Send(const std::string& id, char cmd, char* buff, int len, std::string& err)
{
    bool rc = false;
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(id, pCon))
    {
        if (cmd == ROMVALUE::COMMAND_ORDER &&
                pCon->Blocked())
        {
            err = "Dest has been blocked";
        }
        else
        {
            if (pCon->Send(buff, len, true) == false)
            {
                err = "Can not send to dest";
            }
            else
            {
                rc = true;
            }
        }
    }
    else if (m_UnConMap.Get(id, pCon))
    {
        err = "Not connected to dest";
    }
    else
    {
        err = "Invalid dest";
    }

    return rc;
}

bool CDestsManager::ProcessMessage(tReqList& reqlist)
{
    CDestRequest* pReq = NULL;
    tReqListIt it = reqlist.begin();
    bool rc = true;

    while (it != reqlist.end())
    {
        pReq = *it;

        switch(pReq->RequestType())
        {
        case  CDestRequest::RECONNECT_CHECK:
            ProcessReconnect();
            CleaningTimer();
            //Connect(pReq->ID());
            break;
            //case  CDestRequest::SESSIONEND_CHECK:
            //  ProcessSessionEndCheck(pReq->now());
            //  break;
        case  CDestRequest::UNBLOCK:
            ProcessBlock(pReq->ID(), false);
            break;
        case  CDestRequest::BLOCK:
            ProcessBlock(pReq->ID(), true);
            break;
        case  CDestRequest::CONNECTED:
            ProcessConnected(pReq->ID());
            break;
       // case  CDestRequest::LOGGEDON:
       //     ProcessLoggedOn(pReq->ID());
       //     break;
        case  CDestRequest::DISCONNECTED:
            ProcessDisconnected(pReq->ID());
            break;
            //case  CDestRequest::RELOADDESTS:
            //  ProcessReloadDests();
            //  break;
        case  CDestRequest::QUERYDEST:
            ProcessQueryDest(pReq->ID());
            break;
        case  CDestRequest::QUERYDESTS:
            ProcessQueryDests();
            break;
        case  CDestRequest::RELOADIPPORT:
            ProcessReloadIpPort(pReq->ID());
            break;
        default:
            rc = false;
            break;
        }

        it++;
        delete pReq; //or having the CThread do this??
    }

    return rc;
}

void CDestsManager::GetAllConnections(std::list<CHostConnection*>& conList)
{
    std::list<CHostConnection*> l_conList;
    m_ConMap.GetAll(l_conList);
    m_UnConMap.GetAll(l_conList);
    std::list<CHostConnection*>::iterator it = l_conList.begin();
    while (it != l_conList.end())
    {
        conList.push_back((CHostConnection*)*it);
        it++;
    }
}

std::string CDestsManager::GetDestRoutes()
{
    std::list<CRomMap<std::string>::tKeyValuePair> keyvalues;
    m_DestIdToRouteId.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<std::string>::tKeyValuePair>::iterator it = keyvalues.begin();
    bool first = true;
    std::stringstream stm;
    std::string value;

    while (it != keyvalues.end())
    {
        value = (*it).value;
        if (first)
        {
            stm << (*it).key << "=" << value;
            first = false;
        }
        else
        {
            stm << ":" << (*it).key << "=" << value;
        }
        it++;
    }

    return stm.str();
}

/*CHostConnection* CDestsManager::GetConnectionFromDestID(const std::string& id)
{
    CHostConnection* pCon = NULL;
    std::string routeID;
    if (m_DestIdToRouteId.Get(id, routeID))
    {
        pCon = GetConnectedDest(routeID);
        if (NULL == pCon)
        {
            pCon = GetDisconnectedDest(routeID);
        }
    }
    return pCon;
}*/

CHostConnection* CDestsManager::GetConnectedDest(const std::string& id)
{
    CHostConnection* pCon = NULL;
    m_ConMap.Get(id, pCon);
    return pCon;
}

CHostConnection* CDestsManager::GetDisconnectedDest(const std::string& id)
{
    CHostConnection* pCon = NULL;
    m_UnConMap.Get(id, pCon);
    return pCon;
}

bool CDestsManager::Connect(CHostConnection* pCon)
{
    bool rc = false;

    if (CConfig::getInstance().Async())
    {
        async_server_con_set* pAsyncServer = const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer());
        if (async_dart_connect(MessageCallback,
                               NULL,
                               0,
                               pCon->Port(),
                               const_cast<char*>(pCon->IP().c_str()),
                               pCon->IP().size(),
                               25,
                               0,
                               7,
                               10,
                               NULL,
                               &ConnectCallback,
                               &parse_rom_message,
                               pCon->args(),
                               init_async_rom_logon,
                               &build_offsets,
                               &DummyLowActivityCallback,
                               0,
                               pAsyncServer, pCon, CConnection::OnSend) >= 0)
        {
            rc = true;
        }
    }
    else
    {
        if (dart_connect(&MessageCallback,
                         NULL,
                         0,
                         pCon->Port(),
                         const_cast<char*>(pCon->IP().c_str()),
                         pCon->IP().size(),
                         25,
                         0,
                         7,
                         10,
                         NULL,
                         &ConnectCallback,
                         &parse_rom_message,
                         pCon->args(),
                         &init_rom_logon,
                         &build_offsets,
                         &DummyLowActivityCallback,
                         0) >= 0)
        {
            rc = true;
        }
    }

    return rc;
}

void CDestsManager::Init()
{
    while (CConfig::getInstance().Ready() == false)
    {
        CUtility::Sleep(1);
    }

    tTimerRequest* pReq = new tTimerRequest;
    pReq->timerhandler = &ReconnectCallback;
    pReq->oneshot = false;
    pReq->anyobj = NULL;
    CTimerServer::getInstance().AddRequest(pReq);
}

void CDestsManager::ProcessReconnect()
{
    if (Running() == false)
    {
        return;
    }

    CHostConnection* pCon = NULL;
    std::list<CRomMap<CHostConnection*>::tKeyValuePair> keyvalues;
    m_UnConMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CHostConnection*>::tKeyValuePair>::iterator it = keyvalues.begin();

    while (it != keyvalues.end())
    {
        pCon = it->value;
        //if (pCon->ShouldDestUp(now))
        if (pCon->ShouldDestUp())
        {
            if (pCon->Connecting() == false &&  //remember: this flag will be turned off by a different thread
                pCon->Connected() == false)     //It is crucial to call Connecting() before Connected() to avoid overreconnect
            {
                pCon->SetConnecting(true);
                if (Connect(pCon) == false)
                {
                    pCon->SetConnecting(false);
                }
            }
        }
        it++;
    }
}

/*
void CDestsManager::ProcessSessionEndCheck(unsigned short now)
{
    CHostConnection* pCon = NULL;
    std::list<CRomMap<CHostConnection*>::tKeyValuePair> keyvalues;
    m_ConMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CHostConnection*>::tKeyValuePair>::iterator it = keyvalues.begin();
    while (it != keyvalues.end())
    {
        pCon = it->value;

        if (now >= pCon->CloseTime())
        {
            std::string destID = it->key;
            pCon->SetBlocked(true);
            CThreadOrderHandler::getInstance().ProcessDestCancels(pCon->Name());

            CCsv csvMsg;
            std::string msg;
            csvMsg.SetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, destID);
            csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::MODIFY_ORDERDEST_ACCESS);
            csvMsg.SetAt(ROMFIELDS::CSV_ACTION_FREAK, 1);
            csvMsg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
            csvMsg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
            csvMsg.GetMsg(msg);
            COats::getInstance().Send(msg);
        }

        it++;
    }
}
*/
bool CDestsManager::ReconnectCallback(time_t now, void* obj)
{
    CDestRequest* pReq = new CDestRequest(CDestRequest::RECONNECT_CHECK, "");
    CDestsManager::getInstance().AddMessage(pReq);
    return true;
}

void CDestsManager::SessionEndBlockCallback(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();
    CHostConnection* pCon = NULL;

    while (it != objs.end())
    {
        pCon =  (CHostConnection*)(*it);

        std::string destID = pCon->Name();
        pCon->SetBlocked(true);
        pCon->SetSessionShouldHaveEnded(true);
        CThreadRomOrdersNew::getInstance().ProcessDestCancels(pCon->Name());

        COats::getInstance().HostConnectionUpdate(*pCon, "");
        it++;
    }

    //if (pCon)
    //{
    //    char key_block[10];
    //    sprintf(key_block, "%d_%d", pCon->CloseTime(), pCon->EndWDay());
    //    CDestsManager::getInstance().RemoveBlockingTimer(key_block);
    //}
}

void CDestsManager::SessionStartUnBlockCallback(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();
    CHostConnection* pCon = NULL;

    while (it != objs.end())
    {
        pCon =  (CHostConnection*)(*it);

        std::string destID = pCon->Name();
        pCon->SetBlocked(false);
        pCon->SetSessionShouldHaveEnded(false);

        COats::getInstance().HostConnectionUpdate(*pCon, "");
        it++;
    }

    //if (pCon)
    //{
    //    char key_unblock[10];
    //    sprintf(key_unblock, "%d_%d", pCon->OpenTime(), pCon->ResetDay());
    //    CDestsManager::getInstance().RemoveUnBlockingTimer(key_unblock);
    //}
}

void CDestsManager::MessageCallback(struct message* m, void* ob)
{
    if (l_live == false)
    {
        return;
    }

    if (m->len - 1 == 0)
    {
        std::string errMsg = "Message size = 1 found";
        struct message msg;
        msg.name = const_cast<char*>(CConfig::getInstance().InstanceName().c_str());
        msg.name_len = CConfig::getInstance().InstanceName().length();
        msg.data = const_cast<char*>(errMsg.c_str());
        msg.len = errMsg.length();
        dart_rolling_log_coming(&msg, msg.name, msg.name_len);
        return;
    }

    dart_rolling_log_coming(m, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());

    if (ROMVALUE::COMMAND_HEARTBEAT == (m->data)[0])
    {
        std::string err;
        std::string destName, destID;
        destName.append(m->name, m->name_len);
        if (CDestsManager::getInstance().GetDestIdFromDestName(destName, destID))
        {
            CDestsManager::getInstance().Send(destID, ROMVALUE::COMMAND_HEARTBEAT, m->data, m->len, err);
        }
    }
    else if (ROMVALUE::COMMAND_RESPONSETOADMIN == (m->data)[0])
    {
        //COatsMsg* pMsg = new COatsMsg(m->data, m->len);
        //COats::getInstance().AddMessage(pMsg);
        CCsv* pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(m->data, m->len);
        int freakCmd;
        if(pCsv->GetAt(ROMFIELDS::CSV_COMMAND_FREAK, freakCmd) &&
           freakCmd == ROMVALUE::EXCHANGEINTERFACE_STATUS_QUERY)
        {
            CDestsManager::getInstance().UpdateExchangeStatus(pCsv);
        }

        CStore<CCsv>::getInstance().Return(pCsv);
    }
    else
    {
        CCsv* pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(m->data, m->len);

        if (CThreadRomOrdersNew::getInstance().ProcessHostUpdate(pCsv) == true)
        {
            CStore<CCsv>::getInstance().Return(pCsv);
        }
    }
}

void CDestsManager::UpdateExchangeStatus(const CCsv* pCsv)
{
    std::string destID;
    int act = 0;

    pCsv->GetAt(ROMFIELDS::CSV_ACTION_FREAK, act);
    pCsv->GetAt(ROMFIELDS::CSV_ORDERDEST, destID);
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(destID, pCon))
    {
        pCon->SetExchangeLoggedOn(act);
        COats::getInstance().HostConnectionUpdate(*pCon, "");
    }
}

void CDestsManager::ConnectCallback(con_obj* co, int is_loggedon, char* log,
                                    int len)
{
    if (l_live == false)
    {
        return; //Callback called after the CDestsManager object was destroyed!!!
    }

    std::string ID;
    CDestsManager::getInstance().GetDestIdFromDestName(get_name(co), ID);

    if (2 == is_loggedon)
    {
        //CDestsManager::getInstance().Connected(ID);
        CDestsManager::getInstance().ProcessConnected(ID);
    }
    else if (1 == is_loggedon)
    {
        struct message msg;
        msg.data = log;
        msg.len = len;
        msg.name = const_cast<char*>(get_name(co));
        msg.name_len = strlen(get_name(co));
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());

        CDestsManager::getInstance().LoggedOn(ID);
        //CDestsManager::getInstance().ProcessLoggedOn(ID);
    }
    else
    {
        //CDestsManager::getInstance().Disconnected(ID);
        CDestsManager::getInstance().ProcessDisconnected(ID);
    }
}

void* CDestsManager::ReloadDestsFromDB(void* pArg)
{
    std::string* pDest = (std::string*)pArg;
    CDestsManager::getInstance().LoadDestsFromDB(true, pDest);

    if (pDest)
    {
        //CDestsManager::getInstance().LoadDestFromDB(pDest);
        delete pDest;
    }

    return NULL;
}


bool CDestsManager::LoadDestsFromDB(bool reload, const std::string* pDest)
{
    bool rc = true;
    queue_t results = create_queue();
    //const tDBLogin& db = CConfig::getInstance().db();
    std::string errStr;
    //char sqlStr[128];

    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    databuf_write(pBuffer, "SELECT destid, end_time, destrouteid, dest_short_name, "
                           "sec_type, open_time, reset_wday, "
                           "end_wday FROM OMDestination WHERE active_flag = 1");

    if (pDest)
    {
        databuf_write(pBuffer, " AND destid = %s", pDest->c_str());
    }

    std::string sqlStr;
    sqlStr.assign(pBuffer->buffer, databuf_unread(pBuffer));
    CBuffStore::getInstance().ReturnB(pBuffer);

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        int nDestID = 0, nRouteID = 0;
        std::string szSessionEndTime, szSecType, szDestName, szSessionOpenTime = "0000";
        unsigned short reset_wday = 7, end_wday = 7;

        unsigned long romSecondsToEnd = CConfig::getInstance().SecondsToClose();

        char szDestID[10], szRouteID[10];
        strcpy(szDestID, "");
        strcpy(szRouteID, "");

        pthread_rwlock_wrlock(&m_lock);

        LoadOfDests();

        if (reload)
        {
            CConfig::getInstance().Reload();
        }

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
                        nDestID = (pVal->g_un).un_int;
                        sprintf(szDestID, "%d", nDestID);
                        break;
                    case 1:
                        if (pVal->arr_len)
                        {
                            szSessionEndTime.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 2:
                        nRouteID = (pVal->g_un).un_int;
                        sprintf(szRouteID, "%d", nRouteID);
                        break;
                    case 3:
                        if (pVal->arr_len)
                        {
                            szDestName.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 4:
                        if (pVal->arr_len)
                        {
                            szSecType.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 5:
                        if (pVal->arr_len)
                        {
                           szSessionOpenTime.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 6:
                        reset_wday = (pVal->g_un).un_int;
                        break;
                    case 7:
                        end_wday = (pVal->g_un).un_int;
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

            std::string routeID;

            if (reload)
            {
                bool updateAdmin = false;

                if (m_DestIdToRouteId.Get(szDestID, routeID))
                {
                    if (routeID.compare(szRouteID) != 0)
                    {
                        updateAdmin = true;
                        m_DestIdToRouteId.Update(szDestID, szRouteID);
                    }
                }
                else
                {
                    updateAdmin = true;
                    m_DestIdToRouteId.Add(szDestID, szRouteID);
                }

                if (updateAdmin)
                {
                    //update OrderMaster
                    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
                    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
                    pCsv->SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::DESTROUTE_UPDATE);
                    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, szDestID);
                    pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, szRouteID);

                    std::string message;
                    pCsv->GetMsg(message);
                    COatsMsg* pMsg = new COatsMsg(message);
                    COats::getInstance().AddMessage(pMsg);
                    CStore<CCsv>::getInstance().Return(pCsv);
                }
            }
            else
            {
                m_DestIdToRouteId.Add(szDestID, szRouteID);
            }

            if (nRouteID == nDestID)
            {
                if (m_DestNameToDestId.Find(szDestName))
                {
                    m_DestNameToDestId.Update(szDestName, szRouteID);
                }
                else
                {
                    m_DestNameToDestId.Add(szDestName, szRouteID);
                }

                if (m_RouteIdToSecType.Find(szDestID))
                {
                    m_RouteIdToSecType.Update(szDestID, szSecType[0]);
                }
                else
                {
                    m_RouteIdToSecType.Add(szDestID, szSecType[0]);
                }


                CHostConnection* pCon = NULL;
                bool newConnection = false;

                if (m_ConMap.Get(szRouteID, pCon) == false &&
                    m_UnConMap.Get(szRouteID, pCon) == false)
                {
                    std::string ip;
                    int port;

                    if (CConfig::getInstance().GetDestIpPort(szDestName, ip, port))
                    {
                        pCon = new CHostConnection(szDestID, szDestName, true, false, &RejectCallBack);
                        pCon->SetAsyncServer(const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer()));
                        pCon->SetDescription(szDestName);
                        pCon->SetIP(ip);
                        pCon->SetPORT(port);
                        m_UnConMap.Add(szRouteID, pCon);
                    }
                   
                    newConnection = true;
                }

                if (pCon)
                {
                    time_t period = 0;

                    if (CConfig::getInstance().ResetType() == 1 &&
                        end_wday == 7) //rom is weekly reset and exchange is daily reset
                    {
                        period = 86400;
                    }

                    char okey_block[10], nkey_block[10], okey_unblock[10], nkey_unblock[10];
                    sprintf(okey_block, "%d_%d", pCon->CloseTime(), pCon->EndWDay());
                    sprintf(okey_unblock, "%d_%d", pCon->OpenTime(), pCon->ResetDay());

                    unsigned short centraltime = atoi(szSessionEndTime.c_str());
                    pCon->SetCloseTime(centraltime);
                    centraltime = atoi(szSessionOpenTime.c_str());
                    pCon->SetOpenTime(centraltime);
                    pCon->SetEndWDay(end_wday);
                    pCon->SetServerResetDay(reset_wday);

                    sprintf(nkey_block, "%d_%d", pCon->CloseTime(), pCon->EndWDay());
                    sprintf(nkey_unblock, "%d_%d", pCon->OpenTime(), pCon->ResetDay());

                    unsigned int delay = 0;
                    CConfig::getInstance().GetDestDeathDelay(szDestName, delay);
                    pCon->SetDeathTime(pCon->CloseTime() + delay);

                    if (true == newConnection)
                    {
                        std::string destID = szRouteID;
                        tServerConInfo conInfo = { 0, reset_wday };
                        m_DestConInfo.Get(destID, conInfo);

                        pCon->SetLastLogonTime(conInfo.lastConTime);
                        pCon->SetServerResetDay(conInfo.resetDay);

                        CRomTimer* pTimer = NULL;
                        unsigned long seconds_ToBlock = SecondsToBlock(*pCon);

                        if (seconds_ToBlock != 0)
                        {
                            //no need to block if ROM will go down first
                            if (romSecondsToEnd > seconds_ToBlock)
                            {
                                if (AddBlockingTimerArg(nkey_block, pCon) == false)
                                {
                                    std::set<void*> args;
                                    args.insert(pCon);
                                    pTimer = new CRomTimer(period, seconds_ToBlock, SessionEndBlockCallback, args);
                                    AddBlockingTimer(nkey_block, pTimer);
                                }
                            }
                        }
                        else //should be blocked now
                        {
                            pCon->SetBlocked(true);
                            pCon->SetSessionShouldHaveEnded(true);
                        }

                        unsigned long seconds_ToUnBlock= SecondsToUnBlock(*pCon);

                        if (seconds_ToUnBlock > 0)
                        {
                            //seconds_ToUnBlock < seconds_ToBlock: seconds_ToUnBlock is to open current session
                            //seconds_ToUnBlock > seconds_ToBlock: seconds_ToUnBlock is to open next session
                            if (seconds_ToUnBlock < seconds_ToBlock)
                            {
                                //It has not reached open time of the current session yet
                                pCon->SetSessionShouldHaveEnded(true);
                                pCon->SetBlocked(true);
                            }

                            if (romSecondsToEnd > seconds_ToUnBlock)
                            {
                                if (AddUnBlockingTimerArg(nkey_unblock, pCon) == false)
                                {
                                    std::set<void*> args;
                                    args.insert(pCon);
                                    pTimer = new CRomTimer(period, seconds_ToUnBlock, SessionStartUnBlockCallback, args);
                                    AddUnBlockingTimer(nkey_unblock, pTimer);
                                }
                            }
                        }
                    }
                    else
                    {
                        unsigned long seconds_ToBlock = 0, seconds_ToUnBlock = 0;

                        if (strcmp(okey_block, nkey_block) != 0)
                        {
                            RemoveBlockingTimerArg(okey_block, pCon);
                            seconds_ToBlock = SecondsToBlock(*pCon);

                            if (seconds_ToBlock != 0)
                            {
                                if (romSecondsToEnd > seconds_ToBlock)
                                {
                                    if (AddBlockingTimerArg(nkey_block, pCon) == false)
                                    {
                                        CRomTimer* pTimer = NULL;
                                        std::set<void*> args;
                                        args.insert(pCon);
                                        pTimer = new CRomTimer(period, seconds_ToBlock, SessionEndBlockCallback, args);
                                        AddBlockingTimer(nkey_block, pTimer);
                                    }
                                }
                            }
                            else //no need to block exchange is already down
                            {    //there is a edge case:death time is not reached!!
                                pCon->SetSessionShouldHaveEnded(true);
                                pCon->SetBlocked(true);
                            }
                        }

                        if (strcmp(okey_unblock, nkey_unblock) != 0)
                        {
                            seconds_ToUnBlock = SecondsToUnBlock(*pCon);
                            seconds_ToBlock = SecondsToBlock(*pCon);

                            RemoveUnBlockingTimerArg(okey_unblock, pCon);

                            if (seconds_ToUnBlock > 0)
                            {
                                if (seconds_ToUnBlock < seconds_ToBlock)
                                {
                                    //It has not reached open time of current session yet
                                    pCon->SetSessionShouldHaveEnded(true);
                                    pCon->SetBlocked(true);
                                }
                                else
                                {
                                    pCon->SetSessionShouldHaveEnded(false);
                                    pCon->SetBlocked(false);
                                }

                                if (romSecondsToEnd > seconds_ToUnBlock)
                                {

                                    if (AddUnBlockingTimerArg(nkey_unblock, pCon) == false)
                                    {
                                        CRomTimer* pTimer = NULL;
                                        std::set<void*> args;
                                        args.insert(pCon);
                                        pTimer = new CRomTimer(period, seconds_ToUnBlock, SessionStartUnBlockCallback, args);
                                        AddUnBlockingTimer(nkey_unblock, pTimer);
                                    }
                                }
                            }
                            else
                            {
                                pCon->SetSessionShouldHaveEnded(false);
                                pCon->SetBlocked(false);
                            }
                        }
                    }
                }
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

bool CDestsManager::ProcessReloadIpPort(const std::string& id)
{
    std::string ip;
    int port;
    CHostConnection* pCon = NULL;
    bool rc = false;
    std::string rid;

    CConfig::getInstance().Reload();

    if (m_UnConMap.Get(id, pCon) == false &&
        m_ConMap.Get(id, pCon) == false)
    {
        m_DestNameToDestId.Get(id, rid);  //id can be typed in(does not have to be picked from drop list
    }
    else
    {
        rid = id;
    }

    if (m_UnConMap.Get(rid, pCon))
    {
        if (CConfig::getInstance().GetDestIpPort(pCon->Description(), ip, port))
        {
            pCon->SetIP(ip);
            pCon->SetPORT(port);
        }
    }
    else if (m_ConMap.Get(rid, pCon))
    {
        if (CConfig::getInstance().GetDestIpPort(pCon->Description(), ip, port))
        {
            pCon->SetIP(ip);
            pCon->SetPORT(port);
            Disconnect(pCon->Description());
        }
    }

    return rc;
}

bool CDestsManager::GetConnection(const std::string& id, CHostConnection*& con)
{
    
    bool rc = false;
    
    if (m_ConMap.Get(id, con))
    {
        rc = true;
    }
    else if (m_UnConMap.Get(id, con))
    {
        rc = true;
    }

    return rc;
}

bool CDestsManager::Connect(const std::string& id)
{
    CHostConnection* pCon = NULL;
    bool rc = false;

    if (m_UnConMap.Get(id, pCon))
    {
        rc = Connect(pCon);
    }

    return rc;
}


void CDestsManager::ClearMaps()
{
    m_DestIdToRouteId.RemoveAll();
    m_DestNameToDestId.RemoveAll();
    m_RouteIdToSecType.RemoveAll();
}


void CDestsManager::RejectCallBack(std::list<struct message*>& msgs)
{
    std::list<struct message*>::iterator it = msgs.begin();
    struct message* pMsg = NULL;

    while (it != msgs.end())
    {
        pMsg = *it;
        CDestsManager::getInstance().Reject(pMsg);
        it++;
    }
}

void CDestsManager::Reject(const struct message* pMsg)
{
    //CCsv* pCsv = new CCsv(pMsg->data, pMsg->len - 1);
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
    pCsv->Set(pMsg->data, pMsg->len);

    char cmd = ROMVALUE::COMMAND_ORDER;
    pCsv->GetAt(ROMFIELDS::CSV_COMMAND, cmd);
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Can not send to exchange because of 'would block'");

    switch(cmd)
    {
    case ROMVALUE::COMMAND_CANCEL:
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
        break;
    case ROMVALUE::COMMAND_REPLACE:
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
        break;
    default:
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
        break;
    }

    if (CThreadRomOrdersNew::getInstance().ProcessHostUpdate(pCsv) == true)
    {
        //delete pCsv;
        CStore<CCsv>::getInstance().Return(pCsv);
    }
}

void* CDestsManager::CancelAllDayOrders(void*)
{
    CDestsManager::getInstance().CancelAllDayOrders();
    return NULL;
}


void CDestsManager::CancelAllDayOrders()
{
    std::list<CRomMap<CHostConnection*>::tKeyValuePair> keyvalues;
    m_ConMap.GetKeyValuePairs(keyvalues);
    std::list<CRomMap<CHostConnection*>::tKeyValuePair>::iterator it = keyvalues.begin();
    CHostConnection* pCon = NULL;

    while (it != keyvalues.end())
    {
        pCon = it->value;
        pCon->SetBlocked(true);
        
        COats::getInstance().HostConnectionUpdate(*pCon, "");
        
        /*CCsv csvMsg;
        std::string msg;
        csvMsg.SetAt(ROMFIELDS::CSV_ORDERDEST_FREAK, it->key);
        csvMsg.SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::MODIFY_ORDERDEST_ACCESS);
        csvMsg.SetAt(ROMFIELDS::CSV_ACTION_FREAK, 1);
        csvMsg.SetAt(ROMFIELDS::CSV_SERVER_FREAK, CConfig::getInstance().sInstance());
        csvMsg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
        csvMsg.GetMsg(msg);
        COats::getInstance().Send(msg);*/
        CThreadRomOrdersNew::getInstance().ProcessDestCancels(pCon->Name());
        it++;
    }
}

//unsigned long CDestsManager::SecondsToBlock(time_t now, unsigned short openTime, unsigned short closeTime, unsigned short endWDay)
//unsigned long CDestsManager::SecondsToBlock(unsigned short openTime, unsigned short closeTime, unsigned short endWDay)
unsigned long CDestsManager::SecondsToBlock(CHostConnection& con)
{
    struct tm tmNow, tmCentralTime;
    time_t now;

    time(&now);
    localtime_r(&now, &tmNow);
    CUtility::AdjustToCentralTime(tmNow, tmCentralTime);
    int secondsDiff = ((con.CloseTime()/100) * 60 + con.CloseTime() % 100) * 60 - ((tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec);
    unsigned short daydiff = 0;

    if (con.EndWDay() == 7)
    {
        if (secondsDiff < 0)
        {
            if (con.OpenTime() > con.CloseTime()) //cross calendar day session
            {
                if (((con.OpenTime()/100) * 60 + con.OpenTime() % 100) * 60 - ((tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec) > 0)
                {
                    secondsDiff = 0; //next open time has not been reached yet
                }
                else
                {
                    daydiff = 1;
                }
            }
            else
            {
                secondsDiff = 0; //already closed, no need to block
            }
        }
    }
    else   //weekly session
    {
        if (tmCentralTime.tm_wday != con.EndWDay())
        {
            daydiff = CUtility::GetDaysDiffFromNextWDay(tmCentralTime.tm_wday, con.EndWDay());
        }
        else
        {
            if (secondsDiff < 0)
            {
                if (con.EndWDay() == con.ResetDay())
                {
                    if (((con.OpenTime()/100) * 60 + con.OpenTime() % 100) * 60 - ((tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec) > 0)
                    {
                        secondsDiff = 0; //next open time has not been reached yet
                        daydiff = 0;
                    }
                    else
                    {
                        daydiff = CUtility::GetDaysDiffFromNextWDay(tmCentralTime.tm_wday, con.EndWDay());
                    }
                }
                else
                {
                    secondsDiff = 0;
                }
            }
        }
    }

    unsigned long rc = secondsDiff + daydiff * 86400;

    return rc;
}

//unsigned long CDestsManager::SecondsToUnBlock(time_t now, unsigned short startTime, unsigned short resetWDay)
//unsigned long CDestsManager::SecondsToUnBlock(unsigned short startTime, unsigned short resetWDay)
unsigned long CDestsManager::SecondsToUnBlock(CHostConnection& con)
{
    struct tm tmNow, tmCentralTime;
    time_t now;

    time(&now);
    localtime_r(&now, &tmNow);
    CUtility::AdjustToCentralTime(tmNow, tmCentralTime);

    //int secondsDiff = ((con.OpenTime()/100) * 60 + con.OpenTime() % 100) * 60 - ((tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60 + tmCentralTime.tm_sec);
    int secondsDiff = ((con.OpenTime()/100) * 60 + con.OpenTime() % 100) * 60 - ((tmCentralTime.tm_hour * 60 + tmCentralTime.tm_min) * 60);
    unsigned short daysdiff = 0;

    //if (con.ResetDay() == 7)
    if (con.EndWDay() == 7)
    {
        if (secondsDiff < 0)
        {
            daysdiff = 1;
        }
    }
    else   //weekly session
    {
        if (con.EndWDay() == con.ResetDay() && secondsDiff > 0)
        {
            daysdiff = 0;
        }
        else
        {
            daysdiff = CUtility::GetDaysDiffFromNextWDay(tmCentralTime.tm_wday, con.ResetDay());
        }
    }

    unsigned long rc = secondsDiff + daysdiff * 86400;

    return rc;
}

bool CDestsManager::GetDestSC(const std::string& destID, tSC& destSC)
{
    bool rc = false;
    CHostConnection* pCon = NULL;

    if (m_ConMap.Get(destID, pCon) == false)
    {
        m_UnConMap.Get(destID, pCon);
    }

    if (pCon)
    {
        destSC.closeTime = pCon->CloseTime();
        destSC.openTime = pCon->OpenTime();
        destSC.resetWDay = pCon->ResetDay();
        rc = true;
    }

    return rc;
}

void CDestsManager::LoadOfDests()
{
    m_ofDests.Clear();
    std::string errStr;
    char sqlStr[128];

    //const tDBLogin& db = CConfig::getInstance().db();
    sprintf(sqlStr, "SELECT DestID from OMDestination where (ex_short_name like 'GLBX%c' or ex_short_name like 'ICE%c') and active_flag = 1",
            '%', '%');

    //sprintf(sqlStr, "select DestID from %s.dbo.OMDestination where sec_type='O'", db.database.c_str());

    
    queue_t results = create_queue();
    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
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

            if (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                if (pVal->type_in_union != DB_NULL)
                {
                    int nDestID = (pVal->g_un).un_int;
                    m_ofDests.Add(nDestID);
                }

                cnt++;
                free(colE);
                //free(pVal);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
        }
    }

    destroy_queue(results);
}

bool CDestsManager::IsOptionOnFuture(int destID, char secType)
{
    bool rc = false;


    if (ROMVALUE::SECTYPE_OPTIONFUTURE == secType)
    {
        rc = true;
    }
    else
    {
        if (ROMVALUE::SECTYPE_OPTION == secType &&
            m_ofDests.Find(destID) == true)
        {
            rc = true;
        }
    }

    return rc;
}