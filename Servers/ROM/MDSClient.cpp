#include "MDSClient.h"
#include "rom_handler.h"
#include "ClientsManager.h"
#include "Config.h"
#include "TimerServer.h"
#include "debug_logger.h"
#include "RomOrderNew.h"


bool CMDSClient::send_to_mds(char *data, unsigned long len)
{
    unsigned long bytes_to_send = len;
    char *d_off = data;
    long ret_code = 1;
    int failed_attempts = 0;
    unsigned long bytes_sent = 0;

    bool rc = true;

    pthread_spin_lock(&m_Lock);

    if (m_con)
    {
        while (bytes_sent < len && ret_code >= 0)
        {
            ret_code = dart_send_message(m_con, d_off, bytes_to_send);
            if (ret_code < 0)
            {
                ret_code = 0;
                ++failed_attempts;
                if (failed_attempts >= 128 || get_disconnect(m_con))
                {
                    rc = false;
                    break;
                }
            }
            bytes_to_send -= ret_code;
            bytes_sent += ret_code;
            d_off += ret_code;
        }
    }

    pthread_spin_unlock(&m_Lock);

    return rc;
}

unsigned long CMDSClient::parse_message(const char *message,
                                            unsigned long len,
                                            int *cut_con, struct parse_con *pc)
{
    unsigned long read_bytes = 0, leftover_len = len;
    unsigned long szsz = sizeof(int), mz = 0;
    int sz = 0;

    CMDSClient::getInstance().SetUnResponsive(false);
    char* d_off = const_cast<char*>(message);

    while (leftover_len > szsz)
    {
        memcpy(&sz, d_off, szsz);
        mz = sz + szsz;
        if (mz > 128)
        {
            char err[128];
            sprintf(err, "abnormal length received from MDS:%ld", mz);
            send_debug_message(err);
        }
        if (mz <= leftover_len)
        {
            char *name = (char *) get_name(pc->co);
            struct message m;
            m.name = name;
            m.name_len = get_name_len(pc->co);
            m.len = mz;
            m.data = d_off;
            pc->callback(&m, pc->book);
            leftover_len -= mz;
            read_bytes += mz;
            d_off += mz;
        }
        else
        {
            break;
        }
    }

    return read_bytes;
}

unsigned long CMDSClient::Dummybuild_offsets(   char *data,
                                        unsigned long size,
                                        unsigned long byte_offset,
                                        unsigned long *seq_num)
{
    *seq_num = 0;
    return size;
}

void CMDSClient::ConnectCallback(con_obj* co, int is_connected,char* log, int len)
{
    if (0 == is_connected)
    {
        CMDSClient::getInstance().ProcessDisconnect();
    }

    
    /*
    switch (is_connected)
    {
        case 2:
            CMDSClient::getInstance().ProcessConnect(co);
            break;
        case 1:
            send_debug_message("LoggedOn to MDS");
            CMDSClient::getInstance().SetConnecting(false);
            break;
        default:
            CMDSClient::getInstance().ProcessDisconnect();
            break;
    }
    */
}

void CMDSClient::ProcessData(const std::string& symbol, const tBestOfferBid& ob, bool onopen)
{
    char dumpStr[128];
    if (false == onopen)
    {
        sprintf(dumpStr, "symbol<%s>:bo<%ld>/bb<%ld>\n", symbol.c_str(), ob.offer, ob.bid);
    }
    else
    {
        sprintf(dumpStr, "symbol<%s>:bo_onopen<%ld>/bb_onopen<%ld>\n", 
                symbol.c_str(), ob.offer, ob.bid);
    }
    
    
    send_debug_message(dumpStr);

    tBestOfferBid obj = {0,0,false};

    CRomMap<tBestOfferBid>* pMap = (onopen)? &m_bestOfferBid_OnOpen : &m_bestOfferBid;
    if (pMap->Get(symbol, obj))
    {
        obj.bid = ob.bid;
        obj.offer = ob.offer;
        pMap->Update(symbol, obj);
    }
    else
    {
        pMap->Add(symbol, ob);
    }
}

void CMDSClient::MessageCallback(struct message* m, void* ob)
{
    char* d_off = m->data;
    char type;
    d_off += sizeof(int);                //move to the type
    memcpy(&type, d_off, sizeof(type));
    
    switch(type)
    {
        case CMD_DATA:
        case CMD_ONOPEN_DATA:
            {
                 d_off++;
                std::string symbol;
                
                size_t symbol_len = m->len - sizeof(char) - sizeof(int) - sizeof(long) * 2;
                symbol.assign(d_off, symbol_len);
                d_off += symbol_len;
                tBestOfferBid bob = {0,0,false};
               
                memcpy(&(bob.bid), d_off, sizeof(bob.bid));
                d_off += sizeof(bob.bid);
                memcpy(&(bob.offer), d_off, sizeof(bob.offer));
                if (CMD_DATA == type)
                {
                    CMDSClient::getInstance().ProcessData(symbol, bob, false);
                }
                else
                {
                    CMDSClient::getInstance().ProcessData(symbol, bob, true);
                }
            }
            break;
        case CMD_LOGON:
            send_debug_message("Logged on to MDS");
            break;
        case CMD_HB:
            send_debug_message("Received HB from MDS");
            break;
    }
}

void CMDSClient::Connect()
{
    //async_server_con_set* pAsyncServer = const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer());
    //const std::string& ip = CConfig::getInstance().mds_ip();
    //int port = CConfig::getInstance().mds_port();
    SetConnecting(true);
/*
    if (pAsyncServer)
    {
        if (async_dart_connect( MessageCallback,
                                NULL,
                                0,
                                port,
                                const_cast<char*>(ip.c_str()),
                                ip.size(),
                                CConfig::getInstance().AdjustedEndHour(),
                                CConfig::getInstance().AdjustedEndMin(),
                                7,
                                10,
                                NULL,
                                &ConnectCallback,
                                &parse_message,
                                &m_args,
                                send_logon,
                                &Dummybuild_offsets,
                                &DummyLowActivityCallback,
                                0,
                                pAsyncServer, NULL, &DummyOnSend) < 0)
        {
            send_debug_message("async_dart_connect failed in CMDSClient");
        }
    }
    else
    {
        if (dart_connect(&MessageCallback,
                         NULL,
                         0,
                         port,
                         const_cast<char*>(ip.c_str()),
                         ip.size(),
                         25,
                         0,
                         7,
                         10,
                         NULL,
                         &ConnectCallback,
                         &parse_message,
                         &m_args,
                         send_logon,
                         &Dummybuild_offsets,
                         &DummyLowActivityCallback,
                         0) == -1)
        {
            send_debug_message("dart_connect failed in CMDSClient");
        }
    }*/
}

CMDSClient::CMDSClient() :
m_bestOfferBid(true),
m_bestOfferBid_OnOpen(true),
m_unresponsive(false),
m_equityExempted(false),
m_optionExempted(false),
m_connecting(false),
m_emailFlag(true),
m_hbTimer(NULL)
{
    m_con = NULL;

    memset(&m_args, 0, sizeof(struct init_args));
    m_args.name_len = 3;
    m_args.name = (char*)calloc(1, m_args.name_len + 1);
    strcpy(m_args.name, "MDS");

    m_args.p_len = m_args.name_len;
    m_args.password = (char*)calloc(1, m_args.p_len + 1);
    strcpy(m_args.password, m_args.name);
    m_args.un_len = m_args.p_len;
    m_args.username = (char*)calloc(1, m_args.un_len + 1);
    strcpy(m_args.username, m_args.name);
    m_args.cancel_only = 0;
    m_args.log_outgoing = 1;

    m_args.reset_time = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
    (m_args.reset_time)->new_incoming = 0;
    (m_args.reset_time)->new_outgoing = 0;
    (m_args.reset_time)->last_logon = 0;
    (m_args.reset_time)->create_new = true;

    pthread_spin_init(&m_Lock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&m_exemptLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&m_emailLock, PTHREAD_PROCESS_PRIVATE);
}

CMDSClient::~CMDSClient()
{
    if (m_hbTimer)
    {
        delete m_hbTimer;
    }

    if (m_args.name)
    {
        free (m_args.name);
    }

    if (m_args.password)
    {
        free (m_args.password);
    }

    if (m_args.username)
    {
        free (m_args.username);
    }

    if (m_args.reset_time)
    {
        free (m_args.reset_time);
    }

    if (m_pConnectTimer)
    {
        delete m_pConnectTimer;
    }

    if (m_pDisconnectTimer)
    {
        delete m_pDisconnectTimer;
    }

    pthread_spin_destroy(&m_Lock);
    pthread_spin_destroy(&m_exemptLock);
    pthread_spin_destroy(&m_emailLock);
}

bool CMDSClient::Connected()
{
    pthread_spin_lock(&m_Lock);
    bool rc = (m_con == NULL)? false : true;
    pthread_spin_unlock(&m_Lock);
    return rc;
}

void CMDSClient::SetConnecting(bool value)
{
    pthread_spin_lock(&m_Lock);
    m_connecting = value;
    pthread_spin_unlock(&m_Lock);
}

bool CMDSClient::Connecting()
{
    pthread_spin_lock(&m_Lock);
    bool rc = m_connecting;
    pthread_spin_unlock(&m_Lock);
    return rc;
}

/*bool CMDSClient::ValidatePrice(const std::string& symbol, bool buy, long price, const char* owner, bool field39, std::string& err)
{
    bool rc = true;

    if (false == m_equityExempted)
    {

        tBestOfferBid ob = {0,0,0,0,false};

        if (Connected() == true)
        {
            if (m_bestOfferBid.Get(symbol, ob))
            {
                if (false == ob.exempted)
                {
                    if (buy)
                    {
                        if (price >= ob.offer)
                        {
                            err.assign("Price is too high");
                            rc = false;
                        }
                    }
                    else
                    {
                        if (price <= ob.bid)
                        {
                            err.assign("Price is too low");
                            rc = false;
                        }
                    }
                }
            }
            else
            {
                if (false == field39)
                {
                    char msg[128];
                    sprintf(msg, "Owner<%s>:symbol<%s> not found in market data", owner, symbol.c_str());
                    send_debug_message(msg);
                }
            }
        }
    }
    return rc;
}*/

bool CMDSClient::ValidatePrice(const CRomOrderNew& order, bool bReplace, long price, bool field39, std::string& err)
{
    bool rc = true;

    if (false == m_equityExempted)
    {

        tBestOfferBid ob = {0,0,false};

        if (Connected() == true)
        {
            CRomMap<tBestOfferBid>* pMap = NULL;
            if (ROMVALUE::TIF_OPG == order.TIF() || 
                ROMVALUE::TIF_EXDAY == order.TIF() ||
                strcmp("INET-O", order.Instruction()) == 0)
            {
                pMap = &m_bestOfferBid_OnOpen;
            }
            else
            {
                pMap = &m_bestOfferBid;
            }
            
            if (pMap->Get(order.Symbol(), ob))
            {
                if (false == ob.exempted)
                {
                    if (order.SideIndex() == ROMVALUE::BUY)
                    {
                        if (ob.offer != 0)
                        {
                            if (price >= ob.offer)
                            {
                                err.assign("Price is too high");
                                rc = false;
                            }
                        }
                        else
                        {
                            char msg[512];
                            sprintf(msg, "Offer price of symbol<%s> is zero in market data", order.Symbol());
                            send_debug_message(msg);
                        }
                    }
                    else
                    {
                        if (ob.bid != 0)
                        {
                            if (price <= ob.bid)
                            {
                                err.assign("Price is too low");
                                rc = false;
                            }
                        }
                        else
                        {
                            char msg[512];
                            sprintf(msg, "Bid price of symbol<%s> is zero in market data", order.Symbol());
                            send_debug_message(msg);
                        }
                    }
                }
            }
            else
            {
                if (false == field39)
                {
                    char msg[512];
                    const char* instruction = (bReplace)? order.ReplacingInstructions() : order.Instruction();
                    sprintf(msg, "Owner<%s> Instruction<%s>:symbol<%s> not found in market data", order.Owner(), instruction, order.Symbol());
                    send_debug_message(msg);
                }
            }
        }
    }
    return rc;
}

/*bool CMDSClient::ValidateOptionPrice(const std::string& trader, bool buy, const double& price, const std::string& nbbo, std::string& err)
{
    bool rc = true;

    if (false == OptionExempted(trader))
    {
        if (nbbo.empty()) //empty CSV 39
        {
            err.assign("CSV field 39 is empty");
            rc = false;
        }
        else
        {
            double dnbbo = atof(nbbo.c_str());
            double priceFactor = CConfig::getInstance().OptPriceFactor();

            if (buy)
            {
                if (price >= priceFactor * dnbbo)
                {
                    err.assign("Price is too high");
                    rc = false;
                }
            }
            else
            {
                if (price <= (2 - priceFactor) * dnbbo)
                {
                    err.assign("Price is too low");
                    rc = false;
                }
            }
        }
    }

    return rc;
}*/

bool CMDSClient::ValidateOptionPrice(const CRomOrderNew& order, bool bReplace, const std::string& nbbo, std::string& err)
{
    bool rc = true;

    if (false == OptionExempted(order.Trader()))
    {
        if (nbbo.empty()) //empty CSV 39
        {
            err.assign("NBBO(CSV field 39) is empty in order message: OrderTag=");
            err.append(order.ClientTag());
            err.append(" owner=");
            err.append(order.Owner());
            send_debug_message(err.c_str());
            rc = false;
        }
        else
        {
            double dnbbo = atof(nbbo.c_str());
            double priceFactor = CConfig::getInstance().OptPriceFactor();
            double price = (bReplace)? order.ReplacingPrice() : order.Price();
            
            if (order.SideIndex() == ROMVALUE::BUY)
            {
                if (price >= (1 + priceFactor) * dnbbo)
                {
                    err.assign("Price is too high");
                    rc = false;
                }
            }
            else
            {
                if (price <= (1 - priceFactor) * dnbbo)
                {
                    err.assign("Price is too low");
                    rc = false;
                }
            }
        }
    }

    return rc;
}
 
//bool CMDSClient::NeedValidatePrice(const CRomOrderNew& order, short type, short tif, char execInst)
bool CMDSClient::NeedValidatePrice(const CRomOrderNew& order, bool bReplace)
{
    bool rc = false;
    
    std::string dest = order.Dest();
    if (CConfig::getInstance().IsPriceCheckExemptedDest(dest) == false)
    {
        short type = (bReplace)? order.ReplacingType() : order.Type();
        //short tif = (bReplace)? order.ReplacingTIF() : order.TIF();
        char execInst = (bReplace)? order.ExecInst() : order.ReplacingExecInst();

        if (order.CplxType() == ROMVALUE::CPLXTYPE_SINGLE &&
            (ROMVALUE::TYPE_LIMIT == type || ROMVALUE::TYPE_STOPLIMIT == type)) //&&
            //!(ROMVALUE::TIF_OPG == tif || ROMVALUE::TIF_EXDAY == tif) &&
            //strcmp("INET-O", order.Instruction()) != 0)
        {
            switch (order.SecIndex())
            {
            case ROMVALUE::OPTION:
                rc = true;
                break;
            case ROMVALUE::EQUITY:
                if (!(order.AlgoType() == 1 || order.AlgoType() == 2 ||
                      execInst == 'P' || execInst == 'M' ||
                      execInst == 'R'))
                {
                    rc = true;
                }
                break;
            }
        }
    }
    return rc;
}

int CMDSClient::send_logon(struct async_parse_args *pc)
{
    parse_con* con = pc->con;
    return send_logon(con);
}

int CMDSClient::send_logon(struct parse_args *pc)
{
    parse_con* con = pc->con;
    return send_logon(con);
}

int CMDSClient::send_logon(parse_con* con)
{
    int ret_val = 1;

    CMDSClient::getInstance().SetCon(con->co, false);

    if (CMDSClient::getInstance().SendCmd(CMD_LOGON))
    {
        con->init_con(con->co);
    }
    else
    {
        set_disconnect(con->co, 1);
        ret_val = 0;
    }

    CMDSClient::getInstance().SetConnecting(false);

    return ret_val;
}

void CMDSClient::ProcessDisconnect()
{
    send_debug_message("Disconnected from MDS\n");
    bool connected = Connected();
    SetCon(NULL, connected);
    CRomMap<tBestOfferBid> dummy1, dummy2;
    m_bestOfferBid.Swap(dummy1);
    m_bestOfferBid_OnOpen.Swap(dummy2);
    SetConnecting(false);
}


void CMDSClient::SetCon(con_obj* con, bool connectedBefore)
{
    pthread_spin_lock(&m_Lock);
    m_con = NULL;
    /*m_con = con;
    
    if (m_con)
    {
        if (EmailFlag() == false)
        {
            SetEmailFlag(true);
            std::stringstream stm;
            stm << CConfig::getInstance().InstanceName()
                << " Connected to MDS";
            std::string msg = stm.str();
            CUtility::SendBizEMail(msg, msg);
        }
    }
    else
    {
        if (connectedBefore)
        {
            if (EmailFlag() &&
                m_shouldlive == true)
            {
                std::stringstream stm;
                stm << CConfig::getInstance().InstanceName()
                    << " disconnected from MDS";
                std::string msg = stm.str();
                CUtility::SendBizEMail(msg, msg);
                SetEmailFlag(false);
            }
        }
        else
        {
            if (EmailFlag())
            {
                std::stringstream stm;
                stm << CConfig::getInstance().InstanceName()
                    << " can not connect to MDS";
                std::string msg = stm.str();
                CUtility::SendBizEMail(msg, msg);
                SetEmailFlag(false);
            }
        }
    }*/
    pthread_spin_unlock(&m_Lock);
}

void CMDSClient::SendHB()
{
    static bool didHB = false;

    if (m_unresponsive.Get() == true)
    {
        if (didHB)
        {
            drop_connection();
            send_debug_message("ROM is disconnecting MDS");
            didHB = false;
        }
        else
        {
            if (SendCmd(CMD_HB))
            {
                send_debug_message("Sent HB to MDS");
            }
            didHB = true;
        }
    }
    else
    {
        didHB = false;
        m_unresponsive.Set(true);
    }
}

void CMDSClient::drop_connection()
{
    pthread_spin_lock(&m_Lock);
    if (m_con)
    {
        async_server_con_set* pAsyncServer = const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer());
        std::string mname = "MDS";
        async_kick_off_connection(pAsyncServer, (char*)mname.c_str(), mname.size());
        //m_con = NULL;
    }
    pthread_spin_unlock(&m_Lock);
}

//bool CMDSClient::reconnect_hb(time_t now, void* obj)
void CMDSClient::reconnect_hb(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();
    CMDSClient* pObj = NULL;

    if (it != objs.end())
    {
        pObj =  (CMDSClient*)(*it);
        pObj->Reconnect_HB();
    }

    //CMDSClient* pMe = (CMDSClient*)obj;
    //pMe->Reconnect_HB();
    //return true;
}

void CMDSClient::Reconnect_HB()
{
//#ifdef DEBUG
    //DumpData();
//#endif

    if (m_shouldlive == false)
    {
        if (Connected())
        {
            drop_connection();
        }
    }
    else
    {
        if (Connected())
        {
            SendHB();
        }
        else
        {
            if (Connecting() == false)
            {
                Connect();
            }
        }
    }
}

void CMDSClient::init()
{
    Load();
    std::set<void*> args;
    args.insert(this);
    m_hbTimer = new CRomTimer(30, 5, reconnect_hb, args);
    //m_hbTimer = new CRomTimer(30, 0, reconnect_hb, args);
    //Connect();
    //tTimerRequest* pReq = new tTimerRequest;
    //pReq->timerhandler = &reconnect_hb;
    //pReq->oneshot = false;
    //pReq->anyobj = this;
    //CTimerServer::getInstance().AddRequest(pReq);
}

bool CMDSClient::SendCmd(char type)
{
    const std::string& rom = CConfig::getInstance().InstanceName();
    char buffer[128];
    char *buf_off = buffer;
    int send_len;
    int rom_len = rom.size();
    send_len = rom_len + sizeof(char);

    memcpy(buf_off, &send_len, sizeof(int));
    send_len += sizeof(int);

    buf_off += sizeof(int);
    memcpy(buf_off, &type, sizeof(type));
    buf_off += sizeof(type);
    memcpy(buf_off, rom.c_str(), rom_len);

    bool rc = send_to_mds(buffer, send_len);

    return rc;
}

void CMDSClient::SetUnResponsive(bool value)
{
    m_unresponsive.Set(value);
}

void CMDSClient::DumpData()
{
    std::list<CRomMap<tBestOfferBid>::tKeyValuePair> keyvalues;
    m_bestOfferBid.GetKeyValuePairs(keyvalues);

    std::list<CRomMap<tBestOfferBid>::tKeyValuePair>::iterator it = keyvalues.begin();
    char dumpStr[128];

    send_debug_message("Dumping market data...\n");
    int cnt = 0;
    while (it != keyvalues.end())
    {
        cnt++;
        std::string symbol = it->key;
        tBestOfferBid ob = it->value;
        sprintf(dumpStr, "symbol<%s>:%ld<bo>/%ld<bb>\n", symbol.c_str(), ob.offer, ob.bid);
        send_debug_message(dumpStr);
        it++;
    }

    keyvalues.clear();
    m_bestOfferBid_OnOpen.GetKeyValuePairs(keyvalues);

    it = keyvalues.begin();
    while (it != keyvalues.end())
    {
        cnt++;
        std::string symbol = it->key;
        tBestOfferBid ob = it->value;
        sprintf(dumpStr, "symbol<%s>:%ld<bo_onopen>/%ld<bb_onopen>\n", symbol.c_str(), ob.offer, ob.bid);
        send_debug_message(dumpStr);
        it++;
    }

    sprintf(dumpStr, "%d symbol/price have been received\n", cnt);
    send_debug_message(dumpStr);
}

void CMDSClient::SetExempted(const std::string& symbol, bool value)
{
    if (symbol.empty())
    {
        m_equityExempted = value;
    }
    else
    {
        tBestOfferBid ob = {0,0,false};
        if (m_bestOfferBid.Get(symbol, ob))
        {
            ob.exempted = value;
            m_bestOfferBid.Update(symbol, ob);
        }
        if (m_bestOfferBid_OnOpen.Get(symbol, ob))
        {
            ob.exempted = value;
            m_bestOfferBid_OnOpen.Update(symbol, ob);
        }
    }
}

void CMDSClient::Load()
{
    int hour = 0, min = 0;

    if (CConfig::getInstance().GetConfig("MDS Connection", "connect_hr", hour) == false)
    {
        hour = 6;
    }

    if (CConfig::getInstance().GetConfig("MDS Connection", "connect_min", min) == false)
    {
        min = 30;
    }

    time_t now = 0;
    time(&now);

    time_t connect_t = CUtility::GetTimeTB(hour, min, 7);

    if (connect_t > now)
    {
        std::set<void*> obj;
        obj.insert(this);
        m_pConnectTimer = new CRomTimer(0, connect_t - now, &StartConnect, obj);
    }
    else
    {
        m_shouldlive = true;
    }

    if (CConfig::getInstance().GetConfig("MDS Connection", "disconnect_hr", hour) == false)
    {
        hour = 19;
    }

    if (CConfig::getInstance().GetConfig("MDS Connection", "disconnect_min", min) == false)
    {
        min = 30;
    }

    time_t disconnect_t = CUtility::GetTimeTB(hour, min, 7);

    if (disconnect_t > now)
    {
        std::set<void*> obj;
        obj.insert(this);
        m_pDisconnectTimer = new CRomTimer(0, disconnect_t - now, &EndConnect, obj);
    }
    else
    {
        m_shouldlive = false;
    }

}

void CMDSClient::StartConnect(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();

    if (it != objs.end())
    {
        CMDSClient* pObj = (CMDSClient*)*it;
        pObj->SetShouldLive(true);
    }
}

void CMDSClient::EndConnect(std::set<void*>& objs)
{
    std::set<void*>::iterator it = objs.begin();

    if (it != objs.end())
    {
        CMDSClient* pObj = (CMDSClient*)*it;
        pObj->SetShouldLive(false);
    }
}


void CMDSClient::SetTraderExempt(const std::string& trader, bool bExempted)
{
    pthread_spin_lock(&m_exemptLock);

    if (trader.empty())
    {
        m_optionExempted = bExempted;
    }
    else
    {
        if (bExempted)
        {
            m_OptionExemptedTraders.insert(trader);
        }
        else
        {
            m_OptionExemptedTraders.erase(trader);
        }
    }

    pthread_spin_unlock(&m_exemptLock);
}

bool CMDSClient::OptionExempted(const std::string& trader)
{
    bool rc = false;

    pthread_spin_lock(&m_exemptLock);

    if (true == m_optionExempted ||
        m_OptionExemptedTraders.find(trader) != m_OptionExemptedTraders.end())
    {
        rc = true;
    }

    pthread_spin_unlock(&m_exemptLock);

    return rc;
}


bool CMDSClient::EmailFlag()
{
    pthread_spin_lock(&m_emailLock);
    bool rc = m_emailFlag;
    pthread_spin_unlock(&m_emailLock);
    return rc;
}

void CMDSClient::SetEmailFlag(bool value)
{
    pthread_spin_lock(&m_emailLock);
    m_emailFlag = value;
    pthread_spin_unlock(&m_emailLock);
}

double CMDSClient::GetEquityMidPrice(const std::string& symbol)
{
    double rc = 0;
    pthread_spin_lock(&m_emailLock);
    tBestOfferBid bob = {0,0,false};
    if (m_bestOfferBid.Get(symbol, bob) && bob.exempted == false)
    {
        rc = (double)(bob.bid + bob.offer);
        rc /= 20000;
    }
    pthread_spin_unlock(&m_emailLock);
    return rc;
    
}
