#include "ClientsManager.h"
#include "Config.h"
#include "ClientProfileManagerNew.h"
#include "rom_handler.h"
#include "ROMTypedef.h"
#include "message_queue.h"
//#include "ThreadOrderHandler.h"
#include "Oats.h"
#include "ThreadRomOrdersNew.h"
#include "RomReject.h"
#include "Utility.h"
#include "BuffStore.h"
#include "Utility.h"
#include "HeavyDuty.h"
//#include "CsvStore.h"
#include "async_server.h"
#include "DBExecutor.h"
#include "DestsManager.h"
#include <sys/time.h>
#include "MDSClient.h"
#include "TimerServer.h"

static bool l_live = false;

CClientsManager::CClientsManager() : 
m_Connections(true),
m_hbTimer(NULL)
{
    l_live = true;
    m_pAsyncServer = NULL;
    m_AsyncArgs = NULL;
    pthread_spin_init(&m_hb_lock, PTHREAD_PROCESS_PRIVATE);
    
    if (CConfig::getInstance().Async())
    {
        //int ret = 0;
        queue_t loginQ = create_queue();
        CConfig& cfg = CConfig::getInstance();
        int* cpus = cfg.Cpus();
        int setaffinity = (cpus == NULL)? 0 : 1;

        m_pAsyncServer = async_start_dart_client_services(&MessageCallback,
                         NULL,
                         0,
                         cfg.ServerPort(),
                         loginQ,
                         NULL,
                         0,
                         cfg.AdjustedEndHour(),
                         cfg.AdjustedEndMin(),
                         7,
                         10,
                         NULL,
                         &LogonCallback,
                         &parse_rom_message,
                         &build_offsets,
                         &DummyLowActivityCallback,
                         &ResetPad,
                         0,
                         cfg.numThreads(),
                         cfg.timeout(), cfg.Cpus(), setaffinity, NULL, NULL);


        destroy_queue(loginQ);
    }
}

void CClientsManager::StartMDS()
{
    CMDSClient::getInstance().init();
}

bool CClientsManager::StartService()
{
    //CMDSClient::getInstance().init();
    
    int ret = 0;
    //ROMTYPEDEF::tLoginList logins;
    //CClientProfileManagerNew::getInstance().GetLoginList(logins);
    std::set<CTraderData*> data;
    CClientProfileManagerNew::getInstance().GetTraderData(data);
    queue_t loginQ = create_queue();
    BuildValidLogins(data, loginQ);

    CConfig& cfg = CConfig::getInstance();

    if (m_pAsyncServer)
    {
        m_AsyncArgs = add_async_listener(   m_pAsyncServer,
                                            &MessageCallback,
                                            NULL,
                                            0,
                                            cfg.ServerPort(),
                                            loginQ,
                                            NULL,
                                            0,
                                            cfg.AdjustedEndHour(),
                                            cfg.AdjustedEndMin(),
                                            7,
                                            10,
                                            NULL,
                                            &LogonCallback,
                                            &parse_rom_message,
                                            &build_offsets,
                                            &DummyLowActivityCallback,
                                            &ResetPad,
                                            0,
                                            NULL,
                                            CConnection::OnSend,
                                            NULL,
                                            NULL,
                                            NULL);

        if (m_AsyncArgs)
        {
            std::list<CClientConnection*> conList;
            m_Connections.GetAll(conList);
            std::list<CClientConnection*>::iterator it = conList.begin();
            CClientConnection* pCon = NULL;

            while (it != conList.end())
            {
                pCon = *it;
                pCon->SetAsyncServer(m_pAsyncServer);
                it++;
            }
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = start_dart_client_services(   &MessageCallback,
                                            NULL,
                                            0,
                                            cfg.ServerPort(),
                                            loginQ,
                                            NULL,
                                            0,
                                            cfg.EndHour(),
                                            cfg.EndMin(),
                                            7,
                                            10,
                                            NULL,
                                            &LogonCallback,
                                            &parse_rom_message,
                                            &build_offsets,
                                            &DummyLowActivityCallback,
                                            &ResetPad,
                                            0
                                        );
    }

    std::set<void*> args;
    args.insert(this);

    m_hbTimer = new CRomTimer(30, 30, ProcessHeartBeat, args);

    //tTimerRequest* pReq = new tTimerRequest;
    //pReq->timerhandler = &ProcessHeartBeat;
    //pReq->oneshot = false;
    //pReq->anyobj = this;
    //CTimerServer::getInstance().AddRequest(pReq);
    
    destroy_queue(loginQ);
    return (ret == 0)? true : false;
}

CClientsManager::~CClientsManager(void)
{
    l_live = false;
    
    if (m_hbTimer) 
    {
        delete m_hbTimer;
    }

    CClientConnection* pCon = NULL;
    while ((pCon = (CClientConnection*)m_Connections.RemoveNext()) != NULL)
    {
        delete pCon;
    }
    pthread_spin_destroy(&m_hb_lock);
}

CClientConnection* CClientsManager::FindConnection(const std::string& login)
{
    CClientConnection* pCon = NULL;
    m_Connections.Get(login, pCon);
    return pCon;
}

bool CClientsManager::Send(const std::string& login, CCsv& msg, std::string& err)
{
    bool rc = false;
    CClientConnection* pCon = FindConnection(login);

    if (pCon)
    {
        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        msg.GetMsgNew(pBuffer);
        rc = pCon->Send(pBuffer->buffer, databuf_unread(pBuffer), true);

        if (true == rc && pCon->HBEnabled() )
        {
            pCon->UpdateSendTimeT();
        }
        
        if (false == rc)
        {
            err = "Send failed";
        }

        CBuffStore::getInstance().ReturnB(pBuffer);
    }
    else
    {
        err = "Can not find connection";
    }

    return rc;
}

bool CClientsManager::Send(const std::string& login, char* pMsg, unsigned long len, std::string& err)
{
    bool rc = false;
    CClientConnection* pCon = FindConnection(login);

    if (pCon)
    {
        rc = pCon->Send(pMsg, len, true);

        if (false == rc)
        {
            err = "Send failed";
        }
    }
    else
    {
        err = "Can not find connection";
    }

    return rc;
}

void CClientsManager::BroadCast(const std::string& msg)
{
    std::list<CClientConnection*> conList;
    if (m_Connections.GetAll(conList))
    {
        std::list<CClientConnection*>::iterator it = conList.begin();
        CClientConnection* pCon = NULL;
        while (it != conList.end())
        {
            pCon = *it;
            pCon->SendCmd(msg);
            it++;
        }
    }
}

void CClientsManager::GetAllConnections(std::list<CClientConnection*>& conList)
{
    std::list<CClientConnection*> l_conList;
    m_Connections.GetAll(l_conList);

    std::list<CClientConnection*>::iterator it = l_conList.begin();
    CClientConnection* pCon = NULL;

    while (it != l_conList.end())
    {
        pCon = *it;
        conList.push_back(pCon);
        it++;
    }
}


bool CClientsManager::GetConnection(const std::string& login, CClientConnection*& con)
{
    bool rc = true;
    con = FindConnection(login);
    if (NULL == con)
    {
        rc = false;
    }
    return rc;
}

void CClientsManager::SetBlocked(const std::string& login, bool value)
{
    CClientConnection* pCon = FindConnection(login);
    if (pCon)
    {
        pCon->SetBlocked(value);
    }
}

void CClientsManager::DestroyClientIniArg(struct init_args* pArg)
{
    free(pArg->name);
    free(pArg->password);
    free(pArg->reset_time);
    free(pArg);
}

//void CClientsManager::BuildValidLogins(ROMTYPEDEF::tLoginList& logins, queue_t loginQ)
void CClientsManager::BuildValidLogins(std::set<CTraderData*>& data, queue_t loginQ)
{
    //ROMTYPEDEF::tLoginListIt it = logins.begin();
    std::set<CTraderData*>::iterator it = data.begin();

    while (it != data.end())
    {
        CTraderData* pTraderData = *it;

        struct init_args* pArgs = (struct init_args*)calloc(1, sizeof(struct init_args));
        pArgs->name_len = pTraderData->Trader().size();
        pArgs->name = (char*)calloc(1, pArgs->name_len + 1);
        strcpy(pArgs->name, pTraderData->Trader().c_str());
        pArgs->p_len = pTraderData->Password().size();
        pArgs->password = (char*)calloc(1, pArgs->p_len + 1);
        strcpy(pArgs->password, pTraderData->Password().c_str());

        pArgs->log_outgoing = 1;

        struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
        
        pResetStruct->reset_day =  pTraderData->ResetDay(); //rLogin.resetday;
        pResetStruct->last_logon = pTraderData->LastLogonTime(); //rLogin.lastLogonTime;
        pResetStruct->create_new = CConfig::getInstance().IsFirstRun();

        pResetStruct->new_incoming = 0;
        pResetStruct->new_outgoing = 0;

        //pResetStruct->reset_hour = CConfig::getInstance().StartTime() / 100;
        //pResetStruct->reset_min = CConfig::getInstance().StartTime() %10;
        pResetStruct->reset_hour = 0;
        pResetStruct->reset_min = 0;

        pArgs->reset_time = pResetStruct;
        enqueue(loginQ, pArgs);
        AddClient(*pTraderData);//AddClient(rLogin.login, rLogin.password, rLogin.msgrateLimit);

        it++;
    }
}

//bool CClientsManager::AddClient(const std::string& login, const std::string& password, int* msgrateLimit)
bool CClientsManager::AddClient(CTraderData& data)
{
    bool rc = true;
    
    CClientConnection* pCon = new CClientConnection(data.Trader(), data.Password(), data.msgrateLimit());
    if (m_Connections.Add(data.Trader(), pCon) == false)
    {
        delete pCon;
        rc = false;
    }
    else
    {
        //pCon->SetHBEnabled(data.HBEnalbed());
        pCon->SetHB(data.GetHB());
        
        if (pCon->HBEnabled())
        {
            m_hbClients.insert(pCon);
        }
    }

    return rc;
}

void CClientsManager::MessageCallback(struct message* m, void* ob)
{
    if (l_live == false)
    {
        return;
    }

    if (m->len - 1 == 0)
    {
        return;
    }

    bool rc = true;
    CCsv* pCsv = NULL;
    std::string owner;
    owner.assign(m->name, m->name_len);
    dart_rolling_log_coming(m, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());

    char cmd = (m->data)[0];

    CClientConnection* pCon = CClientsManager::getInstance().FindConnection(owner);
        
    if (pCon && pCon->HBEnabled())
    {
        pCon->UpdateRcvTimeT();
    }
    //if (pCon->DoMsgRateControl())
    //{
    //    pCon->IncrementIncomingCnt();
    //}

    CThreadRomOrdersNew& theHandler = CThreadRomOrdersNew::getInstance();
    std::vector<CCsv*> children;

    char omTime[22];
    CUtility::GetGmtTimeStr(omTime, sizeof(omTime));

    if (cmd == ROMVALUE::COMMAND_ORDER)
    {
        bool hasChildren = (memcmp(m->data + m->len - 4, "!#!", 3) == 0)? true : false;

        if (hasChildren)
        {
            char* pCur = const_cast<char*>(strstr(m->data, "!#!"));
            unsigned int len = pCur - m->data;
            pCsv = CStore<CCsv>::getInstance().Get();
            pCsv->Set(m->data, len);
            pCsv->SetAt(ROMFIELDS::CSV_OWNER, owner);
            pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

            pCur += 3;
            len = m->len - len - 3;
            std::string l_sub(pCur, len);
            std::size_t idx = 0;
            std::string::size_type pos = 0;
            int length = 0;
            CCsv* pChildCsv = NULL;

            while ((idx = l_sub.find("!#!", pos)) != std::string::npos)
            {
                length = idx - pos;
                std::string childStr = l_sub.substr(pos, length);
                pChildCsv = CStore<CCsv>::getInstance().Get();
                pChildCsv->Set(childStr);
                pChildCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
                //CCsv* pCsv = new CCsv(childStr);
                children.push_back(pChildCsv);
                pos += length + 3;
                if (pos >= len)
                {
                    break;
                }
            }
        }
        else
        {
            pCsv = CStore<CCsv>::getInstance().Get();
            pCsv->Set(m->data, m->len - 1);
            pCsv->SetAt(ROMFIELDS::CSV_OWNER, owner);
            pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
        }

        if (pCon != NULL &&
            pCon->Blocked() == true)
        {
            std::string err = "You are blocked";
            CRomReject reject(*pCsv, err);
            std::string MSG;
            reject.GetMsg(MSG);
            pCon->SendCmd(MSG);
            rc = true;
        }
        else
        {
            int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
            pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

            std::string routeID, destID;

            pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
            CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
            pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

            //use the sectype of OMDestination table to
            //determine OptionOnFuture
            //char sectype = ROMVALUE::SECTYPE_EQUITY;
            //CDestsManager::getInstance().GetSecTypeFromRouteId(routeID, sectype);
            //if (ROMVALUE::SECTYPE_OPTIONFUTURE == sectype)
            //{
            //    pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype);
            //}

            switch (cplxtype)
            {
            case ROMVALUE::CPLXTYPE_SINGLE:
                rc = theHandler.ProcessSingleOrder(pCsv, pCon);
                break;
            case ROMVALUE::CPLXTYPE_SPREAD:
                rc = theHandler.ProcessUserDefinedSpreadOrder(pCsv, &children, pCon);
                break;
            case ROMVALUE::CPLXTYPE_PRODUCT_SPREAD:
                rc = theHandler.ProcessSingleOrder(pCsv, pCon);
                break;
            default:
                rc = theHandler.ProcessSingleOrder(pCsv, pCon);
                break;
            }
        }
    }
    else
    {
        pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(m->data, m->len - 1);
        pCsv->SetAt(ROMFIELDS::CSV_OWNER, owner);
        pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

        switch(cmd)
        {
        case ROMVALUE::COMMAND_CANCEL:
            pCsv->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 1);
            rc = theHandler.ProcessClientCancel(pCsv, pCon);
            break;
        case ROMVALUE::COMMAND_CANCELS:
            pCsv->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 1);
            rc = theHandler.ProcessClientCancels(pCsv);
            break;
        case ROMVALUE::COMMAND_REPLACE:
            rc = theHandler.ProcessClientReplace(pCsv, pCon);
            break;
        case ROMVALUE::COMMAND_HEARTBEAT:
            if (pCon->HBEnabled() == false)
            {
                std::string owner;
                pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
                std::string err;
                CClientsManager::getInstance().Send(owner, *pCsv, err);
            }
            rc = true;
            break;
        }
    }

    if (rc)
    {
        CStore<CCsv>::getInstance().Return(pCsv);
        for (size_t i = 0; i < children.size(); i++)
        {
            pCsv = children[i];
            CStore<CCsv>::getInstance().Return(pCsv);
        }
    }
}

void CClientsManager::LogonCallback(con_obj* co, int is_loggedon, char* log, int len)
{
    if (l_live == false)
    {
        return;
    }

    std::string login;
    login.assign(get_name(co), get_name_len(co));
    std::string loginMsg;

    if (1 == is_loggedon)
    {
        if (NULL != log &&
                0 != len)
        {
            loginMsg.append(log, len);
        }
    }

    CClientsManager::getInstance().UpdateConnection(login, get_sock(co), is_loggedon, loginMsg);
}

void CClientsManager::UpdateConnection(const std::string& login, int sock, int updateCode, const std::string& logonMsg)
{
    CClientConnection* pCon = NULL;
    struct message msg;
    char buf[128];

    if (m_Connections.Get(login, pCon))
    {
        //pCon->ResetHBCnt();
        
        if (2 == updateCode)
        {
            pCon->SetConnected(true);
            pCon->SetIpPort(sock);
            sprintf(buf, "Connected from %s(%s)", login.c_str(), pCon->IP().c_str());
            msg.data = buf;
            msg.len = strlen(buf);
        }
        else if (1 == updateCode)
        {
            pCon->UpdateSendTimeT(); //logon confirmation was just sent
            pCon->UpdateRcvTimeT();
            std::string cmd;
            CUtility::GetCsvField(logonMsg.c_str(), ROMFIELDS::CSV_COMMAND, cmd);

            if (cmd[0] == ROMVALUE::COMMAND_PASSWORD_UPDATE)
            {
                std::string newPwd;
                CUtility::GetCsvField(logonMsg.c_str(), ROMFIELDS::CSV_NEWPASSWORD, newPwd);
                tDuty duty;
                duty.theFunc = CClientsManager::ValidateNewPassword;
                tPWDCHG* pPwdChg = new tPWDCHG;
                pPwdChg->login = login;
                pPwdChg->newPassword = newPwd;
                pPwdChg->logonMsg = logonMsg;
                duty.pArg = pPwdChg;
                duty.tmp = false;
                CHeavyDuty::getInstance().AddMessage(duty);
            }
            else
            {

                CCsv logon(logonMsg);

                int cancelOnDisconnect = 1;
                logon.GetAt(ROMFIELDS::CSV_CANCEL_BOOL, cancelOnDisconnect);
                pCon->SetCancelOnDisconnect(cancelOnDisconnect);

                std::string seq;
                if (logon.GetAt(ROMFIELDS::CSV_NEXTEXPECTED_SEQ, seq))
                {
                    pCon->SetDoSequencing(true);
                }
                else
                {
                    //need to reject the order/cancel/replace in case queued up messages because of 'would block' condition
                }

                pCon->SetLoggedOn(true);
                //AddLastLogin(login);
            }

            msg.data = const_cast<char*>(logonMsg.c_str());
            msg.len = logonMsg.length();
        }
        else
        {
            pCon->SetLoggedOn(false);
            pCon->SetConnected(false);
            //Set DoSequencing to false since we don't know if client's next instance will be
            //Sequencing ready or not!!!
            pCon->SetDoSequencing(false);

            if (pCon->CancelOnDisconnect())
            {
                bool dayorderonly = (pCon->CancelOnDisconnect() == 2)? true : false;
                CCsv cancels;
                cancels.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCELS);
                cancels.SetAt(ROMFIELDS::CSV_OWNER, pCon->Name());
                char omTime[22];
                CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
                cancels.SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
                CThreadRomOrdersNew::getInstance().ProcessClientCancels(&cancels, dayorderonly);
            }

            sprintf(buf, "Disconnected from %s(%s)", login.c_str(), pCon->IP().c_str());
            msg.data = buf;
            msg.len = strlen(buf);
        }

        COats::getInstance().ClientConnectionUpdate(*pCon, "");

        msg.name = const_cast<char*>(login.c_str());
        msg.name_len = login.length();
        dart_rolling_log_coming(&msg, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());
    }
}

bool CClientsManager::Find(const std::string& login)
{
    return m_Connections.Find(login);
}

bool CClientsManager::Find(const std::string& login, bool& loggedOn)
{
    bool rc = true;

    //pthread_mutex_lock(&m_lock);

    CClientConnection* pCon = NULL;
    if (m_Connections.Get(login, pCon))
    {
        loggedOn = pCon->LoggedOn();
    }
    else
    {
        rc = false;
    }

    //pthread_mutex_unlock(&m_lock);

    return rc;
}

void CClientsManager::Disconnect(const std::string& login)
{
    CClientConnection* pCon = NULL;
    if (m_Connections.Get(login, pCon))
    {
        //update admin
        pCon->kickoff_connection(const_cast<char*>(login.c_str()), login.size());
        std::string dummy;
        UpdateConnection(login, -1, -1, dummy);
    }
}

struct init_args* CClientsManager::CreateClientIniArg(const ROMTYPEDEF::tLoginStruct& login, bool newcon)
{
    struct init_args* pIniArg = (struct init_args*)calloc(1, sizeof(struct init_args));
    pIniArg->name_len = login.login.size();
    pIniArg->name = (char*)calloc(1, pIniArg->name_len + 1);
    strcpy(pIniArg->name, login.login.c_str());
    pIniArg->p_len = login.password.size();
    pIniArg->password = (char*)calloc(1, pIniArg->p_len + 1);
    struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
    pResetStruct->create_new = newcon;
    pResetStruct->reset_hour = 0;
    pResetStruct->reset_min = 0;
    pResetStruct->new_incoming = 0;
    pResetStruct->new_outgoing = 0;

    pIniArg->reset_time = pResetStruct;

    return pIniArg;
}

void CClientsManager::GetLoggedOnLogins(std::string& logins)
{
    std::list<CClientConnection*> conList;
    m_Connections.GetAll(conList);
    std::list<CClientConnection*>::iterator it = conList.begin();
    CClientConnection* pCon = NULL;

    while (it != conList.end())
    {
        pCon = *it;
        if (pCon->LoggedOn())
        {
            if (it != conList.begin())
            {
                logins += ":";
            }

            logins += pCon->Name();
        }
        it++;
    }
}

//void CClientsManager::AddNewClient(const std::string& name, const std::string& pwd, int* msgrateLimit)
void CClientsManager::AddNewClient(CTraderData& data)
{
    AddClient(data);

    struct init_args* pArgs = (struct init_args*)calloc(1, sizeof(struct init_args));
    pArgs->name_len = data.Trader().size();
    pArgs->name = (char*)calloc(1, pArgs->name_len + 1);
    strcpy(pArgs->name, data.Trader().c_str());
    pArgs->p_len = data.Password().size();
    pArgs->password = (char*)calloc(1, pArgs->p_len + 1);
    strcpy(pArgs->password, data.Password().c_str());

    struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
    pResetStruct->create_new = true;

    pResetStruct->new_incoming = 0;
    pResetStruct->new_outgoing = 0;

    pResetStruct->reset_hour = CUtility::GetLocalTimeHH();
    pResetStruct->reset_min = CUtility::GetTimeMM() - 1;
    pArgs->reset_time = pResetStruct;

    if (m_pAsyncServer)
    {
        add_new_user(m_pAsyncServer, m_AsyncArgs, pArgs->name, pArgs->name_len, pArgs);
    }
    else
    {
        add_config(pArgs);
    }

    free (pArgs->name);
    free (pArgs->password);
    free (pArgs->reset_time);
    free (pArgs);
}

void CClientsManager::UpdateClientPassword(const std::string& name, const std::string& pwd)
{
    struct init_args* pArgs = (struct init_args*)calloc(1, sizeof(struct init_args));
    pArgs->name_len = name.size();
    pArgs->name = (char*)calloc(1, pArgs->name_len + 1);
    strcpy(pArgs->name, name.c_str());
    pArgs->p_len = pwd.size();
    pArgs->password = (char*)calloc(1, pArgs->p_len + 1);
    strcpy(pArgs->password, pwd.c_str());

    struct seq_reset_time* pResetStruct = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));

    CClientConnection* pCon = NULL;

    if (m_Connections.Get(name, pCon))
    {
        pCon->SetPassword(pwd);
        pResetStruct->create_new = !pCon->relogon();
    }
    else
    {
        pResetStruct->create_new = true;
    }

    pResetStruct->new_incoming = 0;
    pResetStruct->new_outgoing = 0;

    pResetStruct->reset_hour = CUtility::GetLocalTimeHH();
    pResetStruct->reset_min = CUtility::GetTimeMM() - 1;
    pArgs->reset_time = pResetStruct;

    if (m_pAsyncServer)
    {
        alter_user( m_pAsyncServer,
                    m_AsyncArgs,
                    name.c_str(),
                    name.size(),
                    pArgs);
    }
    else
    {
        update_config(pArgs);
    }

    free (pArgs->name);
    free (pArgs->password);
    free (pArgs->reset_time);
    free (pArgs);
}

void* CClientsManager::ValidateNewPassword(void* pData)
{
    tPWDCHG* pPwdChg = (tPWDCHG*)pData;
    std::string pwd;
    CDBExecutor* pExecutor = CConfig::getInstance().dbexecutor();
    if ((pPwdChg->newPassword).empty() == false &&
        CUtility::GetPasswordFromDB(*pExecutor, pPwdChg->login, pwd) &&
        pwd.compare(pPwdChg->newPassword) == 0)
    {
        CClientsManager::getInstance().UpdateClientPassword(pPwdChg->login, pPwdChg->newPassword);
        CClientsManager::getInstance().ProcessConfirmPasswordUpdate(pPwdChg->logonMsg);
    }
    else
    {
        CClientsManager::getInstance().ProcessInvalidPasswordUpdate(pPwdChg->logonMsg);
    }

    delete pPwdChg;
    return NULL;
}

void CClientsManager::ProcessInvalidPasswordUpdate(const std::string& logonMsg)
{
    std::string login;

    CCsv logon(logonMsg);
    logon.GetAt(ROMFIELDS::CSV_TRADER, login);
    logon.SetAt(ROMFIELDS::CSV_TEXT, "Invalid Password!");
    logon.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_PASSWORD_UPDATE_REJECT);
    std::string msg;
    logon.GetMsg(msg);
    CClientConnection* pCon = NULL;
    if (m_Connections.Get(login, pCon))
    {
        std::string msg;
        logon.GetMsg(msg);
        pCon->SetLoggedon(true);
        std::string err;
        Send(login, logon, err);
        //pCon->kickoff_connection(const_cast<char*>(login.c_str()), login.length());
        pCon->SetLoggedon(false);
    }
}

void CClientsManager::ProcessConfirmPasswordUpdate(const std::string& logonMsg)
{
    std::string login;

    CCsv logon(logonMsg);
    logon.GetAt(ROMFIELDS::CSV_TRADER, login);
    logon.SetAt(ROMFIELDS::CSV_TEXT, "Password Updated");

    std::string msg;
    logon.GetMsg(msg);
    CClientConnection* pCon = NULL;
    if (m_Connections.Get(login, pCon))
    {
        std::string msg;
        logon.GetMsg(msg);
        pCon->SetLoggedon(true);
        std::string err;
        Send(login, logon, err);
        pCon->SetLoggedon(false);
    }
}

bool CClientsManager::SetMsgRateExempt(const std::string& login, bool exempt)
{
    bool rc = false;
    CClientConnection* pCon = FindConnection(login);
    if (pCon != NULL)
    {
        pCon->SetMsgRateLimitExempt(exempt);
        rc = true;
    }
    return rc;
}

bool CClientsManager::UpdateMsgRateLimits(const std::string& trader, int* limits)
{
    bool rc = false;
    CClientConnection* pCon = FindConnection(trader);
    if (pCon != NULL)
    {
        pCon->UpdateMsgRateLimits(limits);
        rc = true;
    }
    return rc;
}

void CClientsManager::ProcessHeartBeat(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();
    CClientsManager* pObj = NULL;

    if (it != objs.end())
    {
        pObj =  (CClientsManager*)(*it);
        time_t now = 0;
        time(&now);
        pObj->ProcessHeartBeat(now);
    }
}

//bool CClientsManager::ProcessHeartBeat(time_t now, void* obj)
//{
//    CClientsManager* pObj = (CClientsManager*)obj;
//    pObj->ProcessHeartBeat(now);
//    return true;
//}

void CClientsManager::ProcessHeartBeat(time_t now)
{
    time_t deltT = 0;
    
    pthread_spin_lock(&m_hb_lock);
    
    std::set<CClientConnection*>::iterator it = m_hbClients.begin();
    CClientConnection* pCon = NULL;
    
    while (it != m_hbClients.end())
    {
        pCon = *it;
        if (pCon->LoggedOn())
        {
            deltT = now - pCon->GetRcvTimeT();
            if (deltT >= pCon->HBAllowedMisses() * pCon->HBInterval())
            {
                //cut connection;
                char msg[256];
                sprintf(msg, "Haven't heard from %s for %lu seconds, disconnect %s",
                        pCon->Name().c_str(), deltT, pCon->Name().c_str());
                send_debug_message(msg);
                Disconnect(pCon->Name());
            }
            else
            {
                deltT = now - pCon->GetSendTimeT();
                unsigned int mInterval = pCon->HBInterval() * 0.8;
                if (0 == mInterval)
                {
                    mInterval = 1;
                }
                if (deltT >= mInterval)
                {
                    //send HB
                    char omTime[22];
                    CUtility::GetGmtTimeStr(omTime, sizeof(omTime));
                    const std::string& owner =pCon->Name();
                    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
                    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_HEARTBEAT);
                    pCsv->SetAt(ROMFIELDS::CSV_OWNER, CConfig::getInstance().InstanceName());
                    pCsv->SetAt(ROMFIELDS::CSV_OM_TIME, omTime);
                    std::string err;
                    Send(owner, *pCsv, err);
                    CStore<CCsv>::getInstance().Return(pCsv);
                }
            }
        }
        it++;
    }
    
    pthread_spin_unlock(&m_hb_lock);
}

void CClientsManager::UpdateHB(const CTraderData& data)
{
    CClientConnection* pCon = NULL;
    if (m_Connections.Get(data.Trader(), pCon))
    {
        pthread_spin_lock(&m_hb_lock);
        
        pCon->SetHB(data.GetHB());
        if (pCon->HBEnabled())
        {
            m_hbClients.insert(pCon);
        }
        else
        {
            std::set<CClientConnection*>::iterator it = m_hbClients.begin();
            while (it != m_hbClients.end())
            {
                if (*it == pCon)
                {
                    m_hbClients.erase(it);
                    break;
                }
                it++;
            }
        }
        
        pthread_spin_unlock(&m_hb_lock);
    }
}