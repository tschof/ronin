#include "PositionManager.h"
#include "romvaluedefs.h"
#include "Store.h"
#include "client_manager.h"
#include "message_queue.h"
#include "Config.h"
#include "rom_handler.h"
#include "Utility.h"
#include "ClientsManager.h"
#include "TimerServer.h"
#include "mailer.h"
#include "Oats.h"
//#include "RomOrderNew.h"
#include "ThreadRomOrdersNew.h"
#include <errno.h>
#include <sys/stat.h>
#include <string>

CPositionManager::CPositionManager() :
m_rdsCon(NULL),
m_rcdb(CConfig::getInstance().rcDb()),
m_posRecapFinished(false)
{
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
    
    if (CConfig::getInstance().PosMgrEnabled())
    {
        const std::string& hhmm = CConfig::getInstance().hhmmLoadEasyBorrowList();
        m_eswTime = CUtility::GetTimeT(hhmm);
        m_tswTime = m_eswTime;

        time_t now = 0;
        time(&now);
        std::set<void*> dummy1, dummy2;
        const std::string& hm1 = CConfig::getInstance().hhmmLoadEasyThreshold();
        const std::string& hm2 = CConfig::getInstance().hhmmLoadLendingLimits();
        time_t t1 = CUtility::GetTimeT(hm1);
        time_t t2 = CUtility::GetTimeT(hm2);
        t1 = (t1 > now)? t1 - now : 0;
        t2 = (t2 > now)? t2 - now : 0;
        if (t1 > 0)
        {
            //one shot timer
            m_pLoadEasyThresholdListTimer = new CRomTimer(0, t1, LoadEasyThresholdList, dummy1); 
        }
        else
        {
            LoadEasyBorrowList();
            LoadThresholdList();
            m_pLoadEasyThresholdListTimer = NULL;
        }
        if (t2 > 0)
        {
            //one shot timer
            m_pLoadLendingLimitTimer = new CRomTimer(0, t2, LoadLendingLimits, dummy1);
        }
        else
        {
            LoadlendingLimit();
            m_pLoadLendingLimitTimer = NULL;
        }
    }
    else
    {
        m_pLoadLendingLimitTimer = NULL;
        m_pLoadEasyThresholdListTimer = NULL;
        m_eswTime = 0;
        m_tswTime = 0;
    }
    LoadClearingAccounts();
}
   
CPositionManager::~CPositionManager()
{
    if (m_rdsCon)
    {
        delete m_rdsCon;
    }
    
    if (m_pLoadLendingLimitTimer)
    {
        delete m_pLoadLendingLimitTimer;
    }
    
    if (m_pLoadEasyThresholdListTimer)
    {
        delete m_pLoadEasyThresholdListTimer;
    }
    
    ClearPositions();
    
    //m_lenderLimit
    std::list<CRomMap<tLendingLimit*>*> llist;
    m_grpToLenderLimit.GetAll(llist);
    std::list<CRomMap<tLendingLimit*>*>::iterator llist_it = llist.begin();
    while (llist_it != llist.end())
    {
        CRomMap<tLendingLimit*>* pSymLimit = *llist_it;
        std::list<tLendingLimit*> limits;
        pSymLimit->GetAll(limits);
        std::list<tLendingLimit*>::iterator itt = limits.begin();
        while (itt != limits.end())
        {
            tLendingLimit* limit = *itt;
            delete limit;
            itt++;
        }
        delete pSymLimit;
        llist_it++;
    }
    
    std::list<CRomMap<tLendingLimit*>*> llist1;
    m_clrToLenderLimit.GetAll(llist1);
    std::list<CRomMap<tLendingLimit*>*>::iterator llist1_it = llist1.begin();
    while (llist1_it != llist1.end())
    {
        CRomMap<tLendingLimit*>* pSymLimit1 = *llist1_it;
        delete pSymLimit1;
        llist1_it++;
    }  
    
    pthread_spin_destroy(&m_lock);
}
 
void CPositionManager::Start()
{
    m_rdsCon = new CRdsConnection();
    Connect();
    tTimerRequest* pReq = new tTimerRequest;
    pReq->timerhandler = &RDSConCheck;
    pReq->oneshot = false;
    pReq->anyobj = NULL;
    CTimerServer::getInstance().AddRequest(pReq);
}

void CPositionManager::LogonCallback_RDS(con_obj* co, int is_loggedon, char* log, int len)
{
    if (1 == is_loggedon)
    {
        CPositionManager::getInstance().ProcessRdsLogon();
        printf("RDS logged on\n");
    }
    else if (is_loggedon != 2)
    {
        CPositionManager::getInstance().ProcessRdsDisconnect();
    }
}

void CPositionManager::MessageCallback_RDS(struct message* m, void* ob)
{
    dart_rolling_log_coming(m, CConfig::getInstance().InstanceName().c_str(), CConfig::getInstance().InstanceName().length());

    char cmd;
    
    if (m->len - 1 == 0)
    {
        return;
    }
    else
    {
        cmd = m->data[0];
    }
    
    
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
    pCsv->Set(m->data, m->len);
    std::string msgtime;
    pCsv->GetAt(ROMFIELDS::CSV_RDS_MSG_TIME, msgtime);
    
    switch (cmd)
    {
        case ROMVALUE::COMMAND_POSITION_UPDATE:
            CPositionManager::getInstance().ProcessPosUpdateFromRDS(*pCsv);
            CPositionManager::getInstance().SetRdsMsgTime(msgtime);
            break;
        case ROMVALUE::COMMAND_ADMIN:
            {
                int fcmd = 0;
                if (pCsv->GetAt(ROMFIELDS::CSV_COMMAND_FREAK, fcmd) &&
                    ROMVALUE::UPDATE_POSITION == fcmd)
                {
                    tPosUpdate posUpdate;
                    pCsv->GetAt(ROMFIELDS::CSV_POS_SYMBOL, posUpdate.symbol);
                    pCsv->GetAt(ROMFIELDS::CSV_POS_CLRACCT, posUpdate.clract);
                    pCsv->GetAt(ROMFIELDS::CSV_POS_POSCHANGE, posUpdate.qty_change);
                    posUpdate.side = ROMVALUE::SIDE_BUY;
                    CPositionManager::getInstance().UpdatePositionOnly(&posUpdate);
                    CPositionManager::getInstance().SetRdsMsgTime(msgtime);
                }
            }   
            break;
        case ROMVALUE::COMMAND_OPENPOSITION_QUERY:
            if (CConfig::getInstance().IsFirstRun() == false)
            {
                CPositionManager::getInstance().LoadPotentialPosFromLiveOrders();
            }
            CPositionManager::getInstance().SetPosRecapFinished();
            CPositionManager::getInstance().SetRdsMsgTime(msgtime);
        default:
            break;
            
    }
    
    CStore<CCsv>::getInstance().Return(pCsv);
}


void CPositionManager::ProcessPositionUpdate(const tPosUpdate* pUpdate)
{
    tPosition* pPos = NULL;
    
    pthread_spin_lock(&m_lock);
    
    if (m_positions.Get(pUpdate->symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = 0;
        pPos->cur_pos = 0;
        m_positions.Add(pUpdate->symbol, pPos);
    }
    
    pPos->cur_pos += pUpdate->qty_change;
    
    CRomMap<tPosition*>* pClrPos = NULL;
    if (m_ClrPositions.Get(pUpdate->clract, pClrPos) == false)
    {
        pClrPos = new CRomMap<tPosition*>;
        m_ClrPositions.Add(pUpdate->clract, pClrPos);
    }
    
    if (pClrPos->Get(pUpdate->symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = 0;
        pPos->cur_pos = 0;
        pClrPos->Add(pUpdate->symbol, pPos);
    }
    pPos->cur_pos += pUpdate->qty_change;
    
    if (m_easyborrowlist.Find(pUpdate->symbol) == false)
    {
        if (pUpdate->side != ROMVALUE::SIDE_BUY ||
            m_thresholdlist.Find(pUpdate->symbol) == false) //position of symbols in threshold list  
                                                            //should not be returned through buying!!!
        {
            CRomMap<tLendingLimit*>* pLenderLimit = NULL;
            tLendingLimit* pLimit = NULL;

            if (m_clrToLenderLimit.Get(pUpdate->clract, pLenderLimit))
            {
                if (pLenderLimit->Get(pUpdate->symbol, pLimit))
                {
                    pLimit->balance -= pUpdate->qty_change;
                    if (pLimit->balance < 0)
                    {
                        pLimit->balance = 0;
                    }
                }
                else
                {
                    char errstr[1024];
                    sprintf(errstr, "clearing account<%s> has not locate for symbol<%s>",
                        pUpdate->clract.c_str(), pUpdate->symbol.c_str());
                    send_debug_message(errstr);
                    //std::string subj = "Position Warning";
                    //CUtility::SendBizEMail(subj, errstr, strlen(errstr));
                }
            }
            else
            {
                char errstr1[1024];
                sprintf(errstr1, "clearing account<%s> has not locate for symbol<%s>",
                        pUpdate->clract.c_str(), pUpdate->symbol.c_str());
                send_debug_message(errstr1);
                //std::string subj1 = errstr1;
                //CUtility::SendBizEMail(subj1, errstr1, strlen(errstr1));
            }
        }
    }

    pthread_spin_unlock(&m_lock);
}

bool CPositionManager::ProcessSSReplace(CRomOrderNew* pOrder, std::string& err)
{
    tPosUpdate tl;
    tl.symbol = pOrder->Symbol();
    tl.side = pOrder->Side();
    tl.clract = pOrder->ClearingAccount();
    tl.qty_change = pOrder->ShareChange();
    
    
    //bool rc = ProcessSSAdd(tl, err);
    bool rc = ProcessSSAddNew(tl, err);
    if (rc)
    {
        pOrder->SetReplacingSide(tl.side);
        pOrder->SetReplacingBorrowed(tl.qty_change);
    }
    return rc;
}


bool CPositionManager::Lending(tPosUpdate& tl, std::string& err)
{
    bool rc = true;
    CRomMap<tLendingLimit*>* pLenderLimit = NULL;
    tLendingLimit* pLimit = NULL;
    if (m_clrToLenderLimit.Get(tl.clract, pLenderLimit))
    {
        if (pLenderLimit->Get(tl.symbol, pLimit))
        {
            if (pLimit->balance + tl.qty_change > pLimit->limit)
            {
                rc = false;
                char errstr1[1024];
                //std::string grp;
                //m_clr2grp.Get(tl.clract, grp);
                sprintf(errstr1, "Exceeds "
                        "lending limit(clearing account<%s> symbol<%s> limit<%ld> balance<%ld>)",
                        tl.clract.c_str(), tl.symbol.c_str(), pLimit->limit, pLimit->balance);
                err.assign(errstr1);
                rc = false;
                send_debug_message(errstr1);
            }
            else
            {
                pLimit->balance += tl.qty_change;
            }
        }
        else
        {
            char errstr2[1024];
            sprintf(errstr2, "symbol<%s> has no locate for "
                    "clearing account<%s> associates with", 
                    tl.symbol.c_str(), tl.clract.c_str());
            err.assign(errstr2);
            rc = false;
        }
    }
    else
    {
        char errstr3[1024];
        sprintf(errstr3, "clearing account<%s> does not associate with"
                " any lending locate for symbol<%s>", tl.clract.c_str(), tl.symbol.c_str());
        err.assign(errstr3);
        rc = false;
    }
    
    return rc;
}


bool CPositionManager::ProcessSSOrder(CRomOrderNew* pOrder, std::string& err)
{
    tPosUpdate tl;
    tl.symbol = pOrder->Symbol();
    tl.side = pOrder->Side();
    tl.clract = pOrder->ClearingAccount();
    tl.qty_change = pOrder->Shares();
    int side_before = pOrder->Side();
    
    //bool rc = ProcessSSAdd(tl, err);
    bool rc = ProcessSSAddNew(tl, err);
    if (rc)
    {
        if (side_before != ROMVALUE::SIDE_SHORT &&
            tl.side == ROMVALUE::SIDE_SHORT &&
            strlen(pOrder->ShortLender()) == 0)
        {
            pOrder->SetShortLender(pOrder->ClearingId());
        }
        pOrder->SetSide(tl.side);
    }
    return rc;
}

bool CPositionManager::ProcessSSAdd(tPosUpdate& tl, std::string& err)
{
    bool rc = true;
    tPosition *pPos = NULL, *pClrPos = NULL;
    long shareToPos = tl.qty_change;
    
    pthread_spin_lock(&m_lock);
    
    CRomMap<tPosition*>* clrPosMap = NULL;
    if (m_ClrPositions.Get(tl.clract, clrPosMap) == false)
    {
        clrPosMap = new CRomMap<tPosition*>;
        m_ClrPositions.Add(tl.clract, clrPosMap);
    }
    if (clrPosMap->Get(tl.symbol, pClrPos) == false)
    {
        pClrPos = new tPosition;
        pClrPos->ini_pos = 0;
        pClrPos->cur_pos = 0;
        clrPosMap->Add(tl.symbol, pClrPos);
    }
    
    if (m_positions.Get(tl.symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = 0;
        pPos->cur_pos = 0;
        m_positions.Add(tl.symbol, pPos);
    }
    
    if (pClrPos->cur_pos < 0 && pPos->cur_pos < 0) //clr short and firm short
    {
        if (m_easyborrowlist.Find(tl.symbol) == false)
        {
            rc = Lending(tl, err);
        }
        else
        {
            tl.qty_change = 0;
        }
    }
    else if (pClrPos->cur_pos >= 0 && pPos->cur_pos < 0) //clr long and firm short 
    {
        if (m_easyborrowlist.Find(tl.symbol) == false)
        {
            if (shareToPos > pClrPos->cur_pos)
	    {
		tl.qty_change = shareToPos - pClrPos->cur_pos;	
            	rc = Lending(tl, err);
	    }
            if (false == rc)
            {
                const std::string& bizgrp = CConfig::getInstance().SpecialMailGrp();
                std::string subj = "Position Violation";
                char msg[2048];
                sprintf(msg, "%s when Clearing Account<%s> long %ld and "
                        "Ronin Trading is short %ld", 
                        err.c_str(), tl.clract.c_str(), 
                        pClrPos->cur_pos - shareToPos, 
                        shareToPos - pPos->cur_pos);
                
                CUtility::SendBizEMail(subj, msg, strlen(msg));
                send_biz_message1(msg, strlen(msg),
                      subj.c_str(), subj.size(), 
                      bizgrp.c_str(), bizgrp.size());
            }
        }
        else
        {
            tl.qty_change = 0;
        }
    }
    //else if (clrShort == true && frmShort == false)
    else if (pClrPos->cur_pos < 0 && pPos->cur_pos >= 0) //clr short and firm long
    {
        if (m_easyborrowlist.Find(tl.symbol) == false)
        {
            rc = Lending(tl, err);
        }
	else 
	{
	    tl.qty_change = 0;
	}
    }
    else
    {
	if (m_easyborrowlist.Find(tl.symbol) == false)
	{
	    if (shareToPos > pClrPos->cur_pos)
	    {
		tl.qty_change = shareToPos - pClrPos->cur_pos;
		rc = Lending(tl, err);
	    }
	}
    }

    if (rc)
    {
        pPos->cur_pos -= shareToPos;
        pClrPos->cur_pos -= shareToPos;
        if (pPos->cur_pos < 0)
	{
	    tl.side = ROMVALUE::SIDE_SHORT;
	}
	else
	{
	    tl.side = ROMVALUE::SIDE_SELL;
	}
    }
   
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::ProcessSSAddNew(tPosUpdate& tl, std::string& err)
{
    bool rc = true;
    tPosition *pPos = NULL, *pClrPos = NULL;
    long shareToPos = tl.qty_change;
    
    pthread_spin_lock(&m_lock);
    
    CRomMap<tPosition*>* clrPosMap = NULL;
    if (m_ClrPositions.Get(tl.clract, clrPosMap) == false)
    {
        clrPosMap = new CRomMap<tPosition*>;
        m_ClrPositions.Add(tl.clract, clrPosMap);
    }
    if (clrPosMap->Get(tl.symbol, pClrPos) == false)
    {
        pClrPos = new tPosition;
        pClrPos->ini_pos = 0;
        pClrPos->cur_pos = 0;
        clrPosMap->Add(tl.symbol, pClrPos);
    }
    
    if (m_positions.Get(tl.symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = 0;
        pPos->cur_pos = 0;
        m_positions.Add(tl.symbol, pPos);
    }
    
    if (m_easyborrowlist.Find(tl.symbol) == false)
    {
        //lending amount is determined by clearing account position!!!
        if (pClrPos->cur_pos > 0)
        {
            if (shareToPos > pClrPos->cur_pos) //otherwise no need to borrow!!!
            {
                tl.qty_change = shareToPos - pClrPos->cur_pos;
                rc = Lending(tl, err);
            }
            
            if (false == rc && pPos->cur_pos < 0) //no locates available and firm is short
            {
                const std::string& bizgrp = CConfig::getInstance().SpecialMailGrp();
                std::string subj = "Position Violation";
                char msg[2048];
                sprintf(msg, "%s when Clearing Account<%s> long %ld and "
                        "Ronin Trading is short %ld", 
                        err.c_str(), tl.clract.c_str(), 
                        pClrPos->cur_pos - shareToPos, 
                        shareToPos - pPos->cur_pos);
                
                CUtility::SendBizEMail(subj, msg, strlen(msg));
                send_biz_message1(msg, strlen(msg),
                      subj.c_str(), subj.size(), 
                      bizgrp.c_str(), bizgrp.size());
            }
        }
        else
        {
            rc = Lending(tl, err);
        }
    }
    
    if (rc)
    {
        //short market is purely determined by firm current position!!!
        pPos->cur_pos -= shareToPos;
        pClrPos->cur_pos -= shareToPos;
        if (pPos->cur_pos < 0)
	{
	    tl.side = ROMVALUE::SIDE_SHORT;
	}
	else
	{
	    tl.side = ROMVALUE::SIDE_SELL;
	}
    }
       
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::LoadClearingAccounts()
{
    m_clrs.clear();
    m_clrsMap.RemoveAll();
    
    bool rc = false;

    queue_t results = create_queue();
    std::string errStr;
    
    //const tDBLogin& db = CConfig::getInstance().db();
    char sqlStr[1024];
    strcpy(sqlStr, "SELECT clearingAccount FROM ClearingActBillingGroupMap WHERE "
            "ronin_trading = 1");

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        rc = true;

        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
       
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);

            if (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                if (pVal->type_in_union != DB_NULL && pVal->arr_len)
                {
                    std::string clr((pVal->g_un).un_array, pVal->arr_len);
                    m_clrs.append(clr);
                    m_clrsMap.Add(clr, true);
                    if (is_empty(results) == 0)
                    {
                        m_clrs.append("|");
                    }
                }

                free(colE);
                gendb_destroy_v(pVal);
            }

            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
   
    return rc;
}

bool CPositionManager::LoadlendingLimit_test()
{
    CRomMap<tLendingLimit*>* pSymToLLimit = new CRomMap<tLendingLimit*>;
    
    tLendingLimit* pLLimit = new tLendingLimit;
    pLLimit->limit = 100;
    pLLimit->balance = 0;
    pSymToLLimit->Add("AAA", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 100;
    pLLimit->balance = 0;
    pSymToLLimit->Add("BBB", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 100;
    pLLimit->balance = 0;
    pSymToLLimit->Add("CCC", pLLimit);
    
    m_grpToLenderLimit.Add("GRP_A", pSymToLLimit);
    m_clrToLenderLimit.Add("CRN16", pSymToLLimit);
    m_clrToLenderLimit.Add("CRONC01", pSymToLLimit);
    m_clrToLenderLimit.Add("CRONC02", pSymToLLimit);
    
    std::vector<std::string>* pClrsV = new std::vector<std::string>;
    m_lgrp2clrs.Add("GRP_A", pClrsV);
    pClrsV->push_back("CRN16");
    pClrsV->push_back("CRONC01");
    pClrsV->push_back("CRONC02");
    
    //m_grp2clrs.Add("GRP_A", "CRN16| CRONC01 and CRONC02");
    //m_clr2grp.Add("CRONC01", "GRP_A");
    //m_clr2grp.Add("CRONC02", "GRP_A");
    //m_clr2grp.Add("CRN16", "GRP_A");
    
    pSymToLLimit = new CRomMap<tLendingLimit*>;
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("AAA", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("BBB", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("CCC", pLLimit);
    
    m_grpToLenderLimit.Add("GRP_B", pSymToLLimit);
    m_clrToLenderLimit.Add("CRN17", pSymToLLimit);
    pClrsV = new std::vector<std::string>;
    m_lgrp2clrs.Add("GRP_B", pClrsV);
    pClrsV->push_back("CRN17");
    //m_grp2clrs.Add("GRP_B", "CRN17");
    //m_clr2grp.Add("CRN17", "GRP_B");
    
    pSymToLLimit = new CRomMap<tLendingLimit*>;
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("AAA", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("BBB", pLLimit);
    
    pLLimit = new tLendingLimit;
    pLLimit->limit = 200;
    pLLimit->balance = 0;
    pSymToLLimit->Add("CCC", pLLimit);
    
    m_grpToLenderLimit.Add("GRP_C", pSymToLLimit);
    m_clrToLenderLimit.Add("CRN18", pSymToLLimit);
    m_clrToLenderLimit.Add("CRN19", pSymToLLimit);
    
    pClrsV = new std::vector<std::string>;
    m_lgrp2clrs.Add("GRP_C", pClrsV);
    pClrsV->push_back("CRN18");
    pClrsV->push_back("CRN19");
    //m_grp2clrs.Add("GRP_C", "CRN18 and CRN19");
    //m_clr2grp.Add("CRN18", "GRP_C");
    //m_clr2grp.Add("CRN19", "GRP_C");
    
    return true;
}

void CPositionManager::LoadEasyBorrowList_test()
{
    bool dummy_true = true;
    m_easyborrowlist.Add("CA", dummy_true);
}

void CPositionManager::ClearEasyBorrowList()
{
    pthread_spin_lock(&m_lock);
    m_easyborrowlist.RemoveAll();
    pthread_spin_unlock(&m_lock);
}

bool CPositionManager::loadEasyBorrowList(const char* fpath, std::string& err)
{
    bool rc = true;
        
    FILE* fp = fopen(fpath, "r");
    
    if (fp)
    {
        ClearEasyBorrowList();
        char buf[256];
        std::string symbol;
       
	fgets(buf, sizeof(buf), fp); //skip the header 
        while (fgets(buf, sizeof(buf), fp))
        {
            CCsv* pCsv = CStore<CCsv>::getInstance().Get();
            pCsv->Set(buf, strlen(buf));
            if (pCsv->GetAt(2, symbol) && symbol.empty() == false)
            {
                AddEasySymbol(symbol, true);
            }
            else
            {
                //rc = false;
                //file format problem
                
            }
            CStore<CCsv>::getInstance().Return(pCsv);
        }
        if (rc == false || (feof(fp) == 0 && rc))
        {
            char *pErr = strerror(errno);
            std::stringstream stm;
            stm << "fgets<"
            << fpath
            << "> failed:"
            << pErr;
            err = stm.str();
            rc = false;
        }
        fclose(fp);
    }
    else
    {
        char *pErr = strerror(errno);
        std::stringstream stm;
        stm << "Open File<"
        << fpath
        << "> failed:"
        << pErr;
        err = stm.str();
        rc = false;
    }
    
    return rc;
}

bool CPositionManager::ReloadEasyBorrowList(time_t now, void* obj)
{
    CPositionManager::getInstance().LoadEasyBorrowList();
    return true;
}

bool CPositionManager::LoadEasyBorrowList_today(std::string& err)
{
    const std::string& filepath_fmt = CConfig::getInstance().BorrowListFullPathFmt();
    char fpath[128];
    std::string theToday;
    CUtility::GetLocalYYYYMMDD(theToday, 0); //today
    sprintf(fpath, filepath_fmt.c_str(), theToday.c_str());
    bool rc = loadEasyBorrowList(fpath, err);
    return rc;
}

bool CPositionManager::LoadEasyBorrowList_past()
{
    bool rc = false;
    const std::string& filepath_fmt = CConfig::getInstance().BorrowListFullPathFmtPast();
    char fpath[128];
    std::string theDay, err;
    
    for (int i = 1; i < 20; i++)
    {
        CUtility::GetLocalYYYYMMDD(theDay, -i); //yesterday
        sprintf(fpath, filepath_fmt.c_str(), theDay.c_str());
        if ((rc = loadEasyBorrowList(fpath, err)))
        {
            //send email to indicate what is being used for now
            std::string subj = "Previous EasyToBorrow list has been loaded successfully";
            send_debug_message(subj.c_str());
            char msg[256];
            const std::string& romname = CConfig::getInstance().InstanceName();
            sprintf(msg, "%s:%s", romname.c_str(), subj.c_str());
            CUtility::SendBizEMail(subj, msg, strlen(msg));
            break;
        }
    }

    if (false == rc)
    {
        //warning no past file only once 
        static bool done = false;
        if (false == done)
        {
            std::string subj = "Can't find Easy To Borrow list file from previous 20 days";
            CUtility::SendBizEMail(subj, subj.c_str(), subj.size());
            done = true;
        }
    }
    
    return rc;
}

void CPositionManager::LoadEasyBorrowList()
{
    static int DONE = 0;
    
    if (DONE)
    {
        return;
    }
    
    std::string err;
    if (LoadEasyBorrowList_today(err))
    {
        DONE = true;
        std::string subj = "EasyToBorrow list has been loaded successfully";
        send_debug_message(subj.c_str());
        char msg[256];
        const std::string& romname = CConfig::getInstance().InstanceName();
        sprintf(msg, "%s:%s", romname.c_str(), subj.c_str());
        CUtility::SendBizEMail(subj, msg, strlen(msg));
    }
    else
    {
        time_t now = 0;
        time(&now);
        
        if (now > m_eswTime)
        {
            //email once!
            static bool done = false;
            if (false == done)
            {
                std::string subj = "Loading EasyToBorrow list failed";
                send_debug_message(subj.c_str());
                char msg[256];
                const std::string& romname = CConfig::getInstance().InstanceName();
                sprintf(msg, "%s:%s", romname.c_str(), subj.c_str());
                CUtility::SendBizEMail(subj, msg, strlen(msg));
                done = true;
            }
            
            m_eswTime = now + CConfig::getInstance().eswPeriod();
        }
        
        if (m_easyborrowlist.Size() == 0)
        {
            LoadEasyBorrowList_past();
        }
    }
    
    static bool timered = false;
    if (false == timered && false == DONE)
    {
        timered = true;
        //add to timer 30 seconds
        tTimerRequest* pReq = new tTimerRequest;
        pReq->timerhandler = &ReloadEasyBorrowList;
        pReq->oneshot = false;
        pReq->anyobj = NULL;
        CTimerServer::getInstance().AddRequest(pReq);
    }
}

/*
void CPositionManager::LoadClrToBillingGrp()
{
    queue_t results = create_queue();
    std::string errStr;
    
    const tDBLogin& db = CConfig::getInstance().db();
    char sqlStr[1024];
    sprintf(sqlStr, "SELECT A.clearingAccount, B.GroupName FROM "
            "%s.dbo.ClearingActBillingGroupMap A, "
            "%s.dbo.BillingGroup B WHERE A.ronin_trading=1 AND "
            "A.groupID=B.GroupID", db.database.c_str(),
            db.database.c_str());

    CDBExecutor* executor = CConfig::getInstance().dbexecutor();
    if (executor->Execute(sqlStr, NULL, 0, results, errStr))
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
       
        while (is_empty(results) == 0)
        {
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            int cnt = 0;
            std::string clr, bilgrp;
            
            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);

                switch(cnt)
                {
                    case 0:
                        if (pVal->type_in_union != DB_NULL && pVal->arr_len)
                        {
                            clr.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 1:
                        if (pVal->type_in_union != DB_NULL && pVal->arr_len)
                        {
                            bilgrp.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    default:
                        break;
                }
                
                cnt++;
                free(colE);
                gendb_destroy_v(pVal);
            }

            m_clr2grp.Add(clr, bilgrp);
            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + strlen(sqlStr) + 20, sizeof(char));
        sprintf(errMsg, "%s failed:%s", sqlStr, errStr.c_str());
        send_debug_message(errMsg);
        free(errMsg);
    }

    destroy_queue(results);
}
*/

void CPositionManager::ClearLgrp2clrs()
{
    std::list<std::vector<std::string>*> clrsvList;
    m_lgrp2clrs.GetAll(clrsvList);
    std::list<std::vector<std::string>*>::iterator it = clrsvList.begin();
    while (it != clrsvList.end())
    {
        delete *it;
        it++;
    }
    m_lgrp2clrs.RemoveAll();
}

bool CPositionManager::LoadlendingLimit()
{
    bool rc = true;
    
    ClearLgrp2clrs();
    
    queue_t results = create_queue();
    std::string errStr;
    std::string sqlStr = CConfig::getInstance().BiProcLocate();
    
    if (m_rcdb.Execute(sqlStr, NULL, 0, results, errStr) == 1)
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        std::string key;

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            std::string grp, symbol;
            long limit = 0;
            
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
                            grp.assign((pVal->g_un).un_array, pVal->arr_len);
                        }
                        break;
                    case 1:
                        if (pVal->arr_len)
                        {
                            symbol.assign((pVal->g_un).un_array, pVal->arr_len);
		                    ReplaceDotWithSlash(symbol);
                        }
                        break;
                    case 2:
                        limit = (pVal->g_un).un_int;
                        break;
                    default:
                        break;
                    }
                }

                cnt++;
                free(colE);
                gendb_destroy_v(pVal);
            }

            CRomMap<tLendingLimit*>* pSymToLLimit = NULL;
            if (m_grpToLenderLimit.Get(grp, pSymToLLimit) == false)
            {
                pSymToLLimit = new CRomMap<tLendingLimit*>;
                m_grpToLenderLimit.Add(grp, pSymToLLimit);
            }
            
            tLendingLimit* pLimit = NULL;
            if (pSymToLLimit->Get(symbol, pLimit) == false)
            {
                pLimit = new tLendingLimit;
                pSymToLLimit->Add(symbol, pLimit);
                pLimit->balance = 0;
            }
            pLimit->limit = limit;
                    
            std::string clr, errstr;
            std::set<std::string> clrs;
            if (GetClrsFromGrp(clrs, grp, errstr))
            {
                std::string sclrs;
                std::set<std::string>::iterator it = clrs.begin();
                std::vector<std::string>* clrs_v = new std::vector<std::string>;
                m_lgrp2clrs.Add(grp, clrs_v);
                
                while (it != clrs.end())
                {
                    clr = *it;
                    clrs_v->push_back(clr);
                    
                    CRomMap<tLendingLimit*>* psymbol2limit = NULL;
                    if (m_clrToLenderLimit.Get(clr, psymbol2limit) == false)
                    {
                        psymbol2limit = new CRomMap<tLendingLimit*>;
                        m_clrToLenderLimit.Add(clr, psymbol2limit);
                    }
                    
                    tLendingLimit* plimit = NULL;
                    if (psymbol2limit->Get(symbol, plimit) == false)
                    {
                        psymbol2limit->Add(symbol, pLimit);
                    }
                    else
                    {
                        plimit->limit = pLimit->limit;
                    }
                                        
                    it++;
                }
            }
             
            free(rowE);
            destroy_queue(row);
        }
        
        send_debug_message("Lending Limits have been loaded");
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 128, sizeof(char));
        sprintf(errMsg, "Calling BI stored procedure '%s' failed:%s", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
        std::string sbj = "Loading Lending Limits failed";
        CUtility::SendBizEMail(sbj, errMsg, strlen(errMsg));
        
        free(errMsg);
        rc = false;
    }

    destroy_queue(results);
   
    return rc;
}

bool CPositionManager::GetClrsFromGrp(std::set<std::string>& clrs, const std::string& grp, std::string& err)
{
    bool rc = true;
    
    queue_t results = create_queue();
    std::string errStr, sqlStr;
    char sqlstr[128];
    
    sprintf(sqlstr, "%s '%s'", CConfig::getInstance().BiProcDept().c_str(), grp.c_str());
    sqlStr.assign(sqlstr);

    if (m_rcdb.Execute(sqlStr, NULL, 0, results, errStr) == 1)
    {
        queue_t row = NULL;
        gendb_v* pVal = NULL;
        element_t rowE, colE;
        std::string grp;

        while (is_empty(results) == 0)
        {
            int cnt = 0;
            rowE = pop_front(results);
            row = (queue_t)(rowE->d);
            std::string clr;

            while (is_empty(row) == 0)
            {
                colE = pop_front(row);
                pVal = (gendb_v*)(colE->d);
                
                if (pVal->type_in_union != DB_NULL)
                {
                    switch(cnt)
                    {
                    case 1:
                        if (pVal->arr_len)
                        {
                            clr.assign((pVal->g_un).un_array, pVal->arr_len);
                            clrs.insert(clr);
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

            free(rowE);
            destroy_queue(row);
        }
    }
    else
    {
        char* errMsg = (char*)calloc(errStr.length() + sqlStr.size() + 128, sizeof(char));
        sprintf(errMsg, "Calling BI stored procedure '%s' failed:%s", sqlStr.c_str(), errStr.c_str());
        send_debug_message(errMsg);
             
        std::string sbj = "Loading Lending Limits failed";
        CUtility::SendBizEMail(sbj, errMsg, strlen(errMsg));
        
        free(errMsg);
        rc = false;
    }

    destroy_queue(results);
    
    return rc;
}


void CPositionManager::ProcessPosUpdateFromRDS(const CCsv& csv)
{
    //no lock is needed since this happens only when dust is settled!
    std::string symbol, clr;
    long pos = 0, ini_pos = 0;
    
    csv.GetAt(ROMFIELDS::CSV_POS_SYMBOL, symbol);
    AppendR(symbol);
    
    csv.GetAt(ROMFIELDS::CSV_POS_CLR, clr);
    csv.GetAt(ROMFIELDS::CSV_POS_POS, pos);
    csv.GetAt(ROMFIELDS::CSV_POS_INIT_POS, ini_pos);

    CRomMap<tPosition*>* ptPos = NULL;
      
    if (m_ClrPositions.Get(clr, ptPos) == false)
    {
        ptPos = new CRomMap<tPosition*>;
        m_ClrPositions.Add(clr, ptPos);
    }
    
    tPosition* pPos = NULL;
    
    if (ptPos->Get(symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = ini_pos;
        pPos->cur_pos = pos;
        ptPos->Add(symbol, pPos);
    }
       
    if (m_positions.Get(symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = ini_pos;
        pPos->cur_pos = pos;
        m_positions.Add(symbol, pPos);
    }
    else
    {
        pPos->cur_pos += pos;
        pPos->ini_pos += ini_pos;
    }
    
    //need to handle lending balance update using current/initial positions here
    if (FoundEasySymbol(symbol) == false)
    {
        long delt = pos - ini_pos;
        if (delt < 0 ||
            (delt > 0 && FoundThresholdSymbol(symbol) == false))
        {
            tPosUpdate update;
            update.symbol = symbol;
            update.clract = clr;
            update.qty_change = -delt;
            update.side = ROMVALUE::SIDE_SHORT;
            //reset lending balance!!
            ResetLendingBalance(&update);
        }
    }
}


void CPositionManager::Connect()
{
    async_server_con_set* pAsyncServer = const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer());
    if (async_dart_connect( MessageCallback_RDS,
                        NULL,
                        0,
                        m_rdsCon->Port(),
                        const_cast<char*>(m_rdsCon->IP().c_str()),
                        m_rdsCon->IP().size(),
                        25,
                        0,
                        7,
                        10,
                        NULL,
                        &LogonCallback_RDS,
                        &parse_rom_message,
                        m_rdsCon->args(),
                        init_async_rom_logon,
                        &build_offsets,
                        &DummyLowActivityCallback,
                        0,
                        pAsyncServer, m_rdsCon, CConnection::OnSend) == 1)
    {
        m_rdsCon->SetConnecting(true);
    }
}

void CPositionManager::ProcessRdsLogon()
{
    m_rdsCon->SetLoggedon(true);
    m_rdsCon->SetConnecting(false);
   
    if (false == m_posRecapFinished) //send query for current positions
    {
        loadPositionFromRDS();
    }
    else //send the total 'N's received
    {
        SendRdsNSeq();
    }
}

void CPositionManager::SendRdsNSeq()
{
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
         
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ADMIN);
    pCsv->SetAt(ROMFIELDS::CSV_POS_CLR, m_clrs);
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::UPDATE_POSITION);
    pCsv->SetAt(ROMFIELDS::CSV_RDS_MSG_TIME, m_rdsMsgTime);
    std::string msg;
    pCsv->GetMsg(msg);
    m_rdsCon->SendCmd(msg);

    CStore<CCsv>::getInstance().Return(pCsv);
}

void CPositionManager::ProcessRdsDisconnect()
{
    m_rdsCon->SetLoggedon(false);
    m_rdsCon->SetConnecting(false);
}


bool CPositionManager::FindClr(const std::string& clr)
{
    return m_clrsMap.Find(clr);
}

bool CPositionManager::RDSConCheck(time_t now, void* obj)
{
    CPositionManager::getInstance().RDSConCheck();
    return true;
}

void CPositionManager::RDSConCheck()
{
    if (m_rdsCon->Connecting() == false &&
        m_rdsCon->Connected() == false)
    {
        Connect();
    }
    else
    {
        //may do HB
    }
}


void CPositionManager::UpdatePositionOnly(const tPosUpdate* pUpdate)
{
    //tPosition* pPos = NULL;
    
    pthread_spin_lock(&m_lock);
    
    updatePositionOnly(pUpdate);

    pthread_spin_unlock(&m_lock);
}

void CPositionManager::updateLendingBalanceOnly(const tPosUpdate* pUpdate)
{
    CRomMap<tLendingLimit*>* pClrLimit = NULL;
        
    if (m_clrToLenderLimit.Get(pUpdate->clract, pClrLimit))
    {
        tLendingLimit* pLimit = NULL;
        if (pClrLimit->Get(pUpdate->symbol, pLimit)) 
        {
            pLimit->balance += pUpdate->qty_change;
        }
    }
}

void CPositionManager::ResetLendingBalance(const tPosUpdate* pUpdate)
{
    CRomMap<tLendingLimit*>* pClrLimit = NULL;
        
    if (m_clrToLenderLimit.Get(pUpdate->clract, pClrLimit))
    {
        tLendingLimit* pLimit = NULL;
        if (pClrLimit->Get(pUpdate->symbol, pLimit)) 
        {
            pLimit->balance = pUpdate->qty_change;
        }
    }
}

void CPositionManager::UpdateLendingBalanceOnly(const tPosUpdate* pUpdate)
{
    pthread_spin_lock(&m_lock);

    updateLendingBalanceOnly(pUpdate);
   
    pthread_spin_unlock(&m_lock);
}

void CPositionManager::ReloadPositionFromRDS()
{
    //Dusts have to be settled before this can proceed!
    //1) Ronin trading stopped
    //2) No further fills for Ronin trading will come from exchanges
    //3) TPOS has received all the executions from OATS
    //No need to rebuild the lending balance!!!
    pthread_spin_lock(&m_lock);
    
    m_posRecapFinished = false;
    ClearPositions();
    LoadClearingAccounts();
        
    //get the potential positions from open order
    //LoadPotentialPosFromLiveOrders(); //do it after the recap is finished from RDS
    
    if (m_rdsCon)
    {
        loadPositionFromRDS();
    }
    else
    {
        Start();
    }
    
    pthread_spin_unlock(&m_lock);
}

void CPositionManager::loadPositionFromRDS()
{
    //ClearPositions();
    //LoadClearingAccounts();
   
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_OPENPOSITION_QUERY);
    pCsv->SetAt(ROMFIELDS::CSV_POS_CLR, m_clrs);
    std::string msg;
    pCsv->GetMsg(msg);
    m_rdsCon->SendCmd(msg);
    CStore<CCsv>::getInstance().Return(pCsv);
}

bool CPositionManager::GetPos(const std::string& symbol, const std::string& clrAcct, tPosition& pos)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
 
    tPosition* pPos = NULL;
    if (clrAcct.empty())
    {
        if (m_positions.Get(symbol, pPos))
        {
            pos = *pPos;
            rc = true;
        }
    }
    else
    {
        CRomMap<tPosition*>* pSymToPos = NULL;
        if (m_ClrPositions.Get(clrAcct, pSymToPos) && 
            pSymToPos->Get(symbol, pPos))
        {
            pos = *pPos;
            rc = true;
        }
    }
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

//bool CPositionManager::ReloadEasyBorrowList(std::string& err)
//{
//    return LoadEasyBorrowList_today(err);
//}


bool CPositionManager::GetLenderGroupLimits(const std::string& grp, const std::string& symbol, std::string& clrs, tLendingLimit& limit)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
    
    CRomMap<tLendingLimit*>* pMap = NULL;
    tLendingLimit* pLimit = NULL;
    if (m_grpToLenderLimit.Get(grp, pMap) && 
        pMap->Get(symbol, pLimit))
    {
        limit.balance = pLimit->balance;
        limit.limit = pLimit->limit;
        
        std::vector<std::string>* pClrsV = NULL;
        if (m_lgrp2clrs.Get(grp, pClrsV))
        {
            for (size_t j = 0; j < pClrsV->size(); j++)
            {
                if (j > 0)
                {
                    if (j != pClrsV->size() - 1)
                    {
                        clrs.append("| ");
                    }
                    else
                    {
                        clrs.append(" and ");
                    }
                }

                clrs.append((*pClrsV)[j]);
            }
        }
        
        //m_grp2clrs.Get(grp, clrs);
        rc = true;
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}
bool CPositionManager::GetClrLenderLimits(const std::string& clr, const std::string& symbol, tLendingLimit& limit)
{
    bool rc = false;
    
    pthread_spin_lock(&m_lock);
    
    CRomMap<tLendingLimit*>* pMap = NULL;
    tLendingLimit* pLimit = NULL;
    if (m_clrToLenderLimit.Get(clr, pMap) && 
        pMap->Get(symbol, pLimit))
    {
        limit.balance = pLimit->balance;
        limit.limit = pLimit->limit;
        rc = true;
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::GetLenderGroupAllLimits(const std::string& grp, std::vector<tLendingLimitDetail>& detail)
{
    bool rc = true;
    std::vector<std::string> symbols;
    GetSymbolsFromGrp(grp, symbols);
    
    if (symbols.size() == 0)
    {
        rc = false;
    }
    else
    {
        for (size_t i = 0; i < symbols.size(); i++)
        {
            tLendingLimitDetail dt;
            dt.symbol = symbols[i];
            if (GetLenderGroupLimits(grp, dt.symbol, dt.clrs, dt.limit))
            {
                detail.push_back(dt);
            }
        }
    }
    
    return rc;
}

void CPositionManager::GetSymbolsFromGrp(const std::string& grp, std::vector<std::string>& symbols)
{
    CRomMap<tLendingLimit*>* pMap = NULL;
    
    pthread_spin_lock(&m_lock);
    
    if (m_grpToLenderLimit.Get(grp, pMap))
    {
        std::set<std::string> keys;
        pMap->GetAllKeys(keys);
        std::set<std::string>::iterator it = keys.begin();
        while (it != keys.end())
        {
            symbols.push_back(*it);
            it++;
        }
    }
    
    pthread_spin_unlock(&m_lock);
}

bool CPositionManager::AddEasySymbol(const std::string& symbol, bool add_remove)
{
    pthread_spin_lock(&m_lock);
  
    bool rc = false;
    
    std::string sym = symbol;
    ReplaceDotWithSlash(sym);
    
    if (add_remove)
    {
        rc = m_easyborrowlist.Add(sym, true);
    }
    else
    {
        bool dummy;
        rc = m_easyborrowlist.Remove(sym, dummy);
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}


bool CPositionManager::FoundEasySymbol(const std::string& symbol)
{
    pthread_spin_lock(&m_lock);
  
    bool rc = m_easyborrowlist.Find(symbol);
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::FoundThresholdSymbol(const std::string& symbol)
{
    pthread_spin_lock(&m_lock);
  
    bool rc = m_thresholdlist.Find(symbol);
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

void CPositionManager::GetLendingGroups(std::string& grps)
{
    std::set<std::string> grpSet;
    
    pthread_spin_lock(&m_lock);
    m_grpToLenderLimit.GetAllKeys(grpSet);
    pthread_spin_unlock(&m_lock);
    
    std::set<std::string>::iterator it = grpSet.begin();
    while (it != grpSet.end())
    {
        grps.append(*it);
        it++;
        if (it != grpSet.end())
        {
            grps.append("|");
        }
    }
}

void CPositionManager::GetGrpSymbols(std::vector<tGrpSymbols>& vgrpsymbols)
{
    std::set<std::string> grpSet;
    
    pthread_spin_lock(&m_lock);
    m_grpToLenderLimit.GetAllKeys(grpSet);
    pthread_spin_unlock(&m_lock);
    
    std::set<std::string>::iterator it = grpSet.begin();
    while (it != grpSet.end())
    {
        tGrpSymbols grpsymbols;
        grpsymbols.grp = *it;
        GetSymbolsFromGrp(*it, grpsymbols.symbols);
        vgrpsymbols.push_back(grpsymbols);
        it++;
    }
}

bool CPositionManager::AddLendingGroupLimit(const std::string& grp, const std::string& symbol, const std::string& clrs, long newlimit, std::string& echo)
{
    bool rc = true;
    char text[256];
   
    pthread_spin_lock(&m_lock);
    
    CRomMap<tLendingLimit*>* pGrpLimit = NULL;

    if (m_grpToLenderLimit.Get(grp, pGrpLimit))
    {
        sprintf(text, "lending group(%s) already exists", grp.c_str());
        echo.assign(text);
        rc = false;
    }
    else
    {   
        pGrpLimit = new CRomMap<tLendingLimit*>;
        m_grpToLenderLimit.Add(grp, pGrpLimit);
        tLendingLimit* pLimit = new tLendingLimit;
        pLimit->balance = 0;
        pLimit->limit = newlimit;
        pGrpLimit->Add(symbol, pLimit);
       
        std::vector<std::string>* clrs_v = new std::vector<std::string>;
        CUtility::spliter(clrs, *clrs_v, '|');
        m_lgrp2clrs.Add(grp, clrs_v);
        
        for (size_t i = 0; i < clrs_v->size(); i++)
        {
            CRomMap<tLendingLimit*>* pClrLimit = NULL;
            if (m_clrToLenderLimit.Get((*clrs_v)[i], pClrLimit) == false)
            {
                pClrLimit = new CRomMap<tLendingLimit*>;
                m_clrToLenderLimit.Add((*clrs_v)[i], pClrLimit);
            }
            
            tLendingLimit* plimit = NULL;
            if (pClrLimit->Get(symbol, plimit) == false)
            {
                pClrLimit->Add(symbol, pLimit);
                //sclrs_v.push_back((*clrs_v)[i]);
            }
            else
            {
                //the clearing account/symbol has belonged to another lending group
                //the text returned to ordermaster will not have that clearing account!
                char msg[512];
                sprintf(msg, "attempting to put clearing account(%s)/symbol(%s) in lending group(%s) "
                        "but it already exists in another lending group",  
                        (*clrs_v)[i].c_str(), symbol.c_str(), grp.c_str());
                send_debug_message(msg);
            }
        }

        /*
        for (int j = 0; j < sclrs_v.size(); j++)
        {
            if (j > 0)
            {
                if (j != sclrs_v.size() - 1)
                {
                    sclrs.append("| ");
                }
                else
                {
                    sclrs.append(" and ");
                }
            }

            sclrs.append(sclrs_v[j]);
        }

        m_grp2clrs.Add(grp, sclrs);
        */
        sprintf(text, "Lending limit(%ld) for Group<%s>/Symbol<%s> has been added",
                newlimit, grp.c_str(), symbol.c_str());

        echo.assign(text);
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}


bool CPositionManager::UpdateLendingGroupLimit(const std::string& grp, const std::string& symbol, long newlimit, std::string& echo)
{
    bool rc = false;
    char text[256];
    std::string symb = symbol;
    
    ReplaceDotWithSlash(symb);
    
    pthread_spin_lock(&m_lock);
    
    CRomMap<tLendingLimit*>* pGrpLimit = NULL;

    if (m_grpToLenderLimit.Get(grp, pGrpLimit) == true)
    {
        tLendingLimit* pLimit = NULL;
        if (pGrpLimit->Get(symb, pLimit) == false)
        {
            //this is an adding
            pLimit = new tLendingLimit;
            pGrpLimit->Add(symb, pLimit);
            pLimit->balance = 0;
            //need to add to m_clrToLenderLimit
            std::vector<std::string>* pClrsV = NULL;
            if (m_lgrp2clrs.Get(grp, pClrsV))
            {
                for (size_t i = 0; i < pClrsV->size(); i++)
                {
                    CRomMap<tLendingLimit*>* pSym2limit = NULL;
                    if (m_clrToLenderLimit.Get((*pClrsV)[i], pSym2limit))
                    {
                        //tLendingLimit* plimit = NULL;
                        if (pSym2limit->Find(symb) == false)
                        {
                            pSym2limit->Add(symb, pLimit);
                        }
                        else
                        {
                            char msg[512];
                            sprintf(msg, "attempting to put clearing account(%s)/symbol(%s) in lending group(%s) "
                                    "but it already exists in another lending group",  
                                    (*pClrsV)[i].c_str(), symb.c_str(), grp.c_str());
                            send_debug_message(msg);
                        }
                    }
                }
            }
        }
        pLimit->limit = newlimit;
        sprintf(text, "Lending limit for Group<%s>/Symbol<%s> has been updated to %ld",
            grp.c_str(), symb.c_str(), newlimit);
        rc = true;
    }
    else
    {
        sprintf(text, "Can not find Lending Group<%s>", grp.c_str());
    }
    
    echo.assign(text);
           
    pthread_spin_unlock(&m_lock);
    
    return rc;
}
    

void CPositionManager::ProcessSSReplaceReject(CRomOrderNew* pOrder)
{
    pthread_spin_lock(&m_lock);
    
    tPosition* pPos = NULL;
    if (m_positions.Get(pOrder->Symbol(), pPos))
    {
        pPos->cur_pos += pOrder->ShareChange();
    }
    
    if (m_easyborrowlist.Find(pOrder->Symbol()) == false)
    {
        if (pOrder->ReplacingBorrowed() > 0)
        {
            CRomMap<tLendingLimit*>* symbol2limit = NULL;
            if (m_clrToLenderLimit.Get(pOrder->ClearingAccount(), symbol2limit))
            {
                tLendingLimit* limit = NULL;
                if (symbol2limit->Get(pOrder->Symbol(), limit))
                {
                    limit->balance -= pOrder->ReplacingBorrowed();
                }
            }
        }
    }
    
    CRomMap<tPosition*>* pClrPos = NULL;
    if (m_ClrPositions.Get(pOrder->ClearingAccount(), pClrPos))
    {
        tPosition* pPos = NULL;
        if (pClrPos->Get(pOrder->Symbol(), pPos))
        {
            pPos->cur_pos += pOrder->ShareChange();
        }
    }
        
    pthread_spin_unlock(&m_lock);
}


void CPositionManager::LoadThresholdList()
{
    static bool DONE = false;
    
    if (DONE)
    {
        return;
    }
    
    std::string err;
    if (LoadThresholdList_current(err))
    {
        DONE = true;
        std::string subj = "Threshold list has been loaded successfully";
        send_debug_message(subj.c_str());
        char msg[256];
        const std::string& romname = CConfig::getInstance().InstanceName();
        sprintf(msg, "%s:%s", romname.c_str(), subj.c_str());
        CUtility::SendBizEMail(subj, msg, strlen(msg));
    }
    else
    {
        time_t now = 0;
        time(&now);
        
        if (now > m_tswTime)
        {
            static bool done = false;
            
            if (false == done)
            {
                std::string subj = "Loading Threshold list failed";
                send_debug_message(subj.c_str());
                char msg[256];
                const std::string& romname = CConfig::getInstance().InstanceName();
                sprintf(msg, "%s:%s:err=%s", romname.c_str(), subj.c_str(), err.c_str());
                CUtility::SendBizEMail(subj, msg, strlen(msg));
                done = true;
            }
            
            m_tswTime = now + CConfig::getInstance().eswPeriod();
        }
        
        //if (m_thresholdlist.Size() == 0)
        //{
        //    LoadThresholdList_past();
        //}
    }
    
    static bool timered = false;
    if (false == timered && false == DONE)
    {
        timered = true;
        //add to timer 30 seconds
        tTimerRequest* pReq = new tTimerRequest;
        pReq->timerhandler = &ReloadThresholdList;
        pReq->oneshot = false;
        pReq->anyobj = NULL;
        CTimerServer::getInstance().AddRequest(pReq);
    }
}

bool CPositionManager::LoadThresholdList_test()
{
    bool rc = true;
    
    pthread_spin_lock(&m_lock);
    
    m_thresholdlist.Add("AAA", true);
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::loadThresholdList(const char* fpath, std::string& err)
{
    bool rc = true;
        
    FILE* fp = fopen(fpath, "r");
    
    if (fp)
    {
        ClearThresholdList();
        char buf[256];
        std::string symbol;
       
	fgets(buf, sizeof(buf), fp); //skip the header 
        while (fgets(buf, sizeof(buf), fp))
        {
            char* p = strchr(buf, '|');
            if (p)
            {
                symbol.assign(buf, p - buf);
                AddThresholdSymbol(symbol, 1);
            }
        }
        if (rc == false || (feof(fp) == 0 && rc))
        {
            char *pErr = strerror(errno);
            std::stringstream stm;
            stm << "fgets<"
            << fpath
            << "> failed:"
            << pErr;
            err = stm.str();
            rc = false;
        }
        fclose(fp);
    }
    else
    {
        char *pErr = strerror(errno);
        std::stringstream stm;
        stm << "Open File<"
        << fpath
        << "> failed:"
        << pErr;
        err = stm.str();
        rc = false;
    }
    
    return rc;
}
    
bool CPositionManager::LoadThresholdList_past()
{
    bool rc = false;
    const std::string& filepath_fmt = CConfig::getInstance().ThresholdListFullPathFmt();
    char fpath[128];
    std::string theDay, err;
    
    for (int i = 1; i < 20; i++)
    {
        CUtility::GetLocalYYYYMMDD(theDay, -i); //yesterday
        sprintf(fpath, filepath_fmt.c_str(), theDay.c_str());
        if ((rc = loadThresholdList(fpath, err)))
        {
            break;
        }
    }

    if (false == rc)
    {
        //warning no past file 
        std::string subj = "Can't find Threshold list file from previous 20 days";
        CUtility::SendBizEMail(subj, subj.c_str(), subj.size());
    }
    
    return rc;
}

void CPositionManager::ClearThresholdList()
{
    pthread_spin_lock(&m_lock);
    m_thresholdlist.RemoveAll();
    pthread_spin_unlock(&m_lock);
}

bool CPositionManager::ReloadThresholdList(time_t now, void* obj)
{
    CPositionManager::getInstance().LoadThresholdList();
    return true;
}

bool CPositionManager::AddThresholdSymbol(const std::string& symbol, bool add_remove)
{
    pthread_spin_lock(&m_lock);
  
    bool rc = false;
    
    std::string sym = symbol;
    ReplaceDotWithSlash(sym);
    
    if (add_remove)
    {
        rc = m_thresholdlist.Add(sym, true);
    }
    else
    {
        bool dummy;
        rc = m_thresholdlist.Remove(sym, dummy);
    }
    
    pthread_spin_unlock(&m_lock);
    
    return rc;
}

bool CPositionManager::LoadThresholdList_current(std::string& err)
{
    const std::string& filepath_fmt = CConfig::getInstance().ThresholdListFullPathFmt();
    char fpath[128];
    std::string theToday;
    bool rc = false;
    
    for (int i = 1; rc != true && i < 21; i++)
    {
        CUtility::GetLocalYYYYMMDD(theToday, -i); //today
        sprintf(fpath, filepath_fmt.c_str(), theToday.c_str());
        rc = loadThresholdList(fpath, err);
        
        if (true == rc && i > 1)
        {
            struct stat st;
            if (stat(fpath, &st) == 0)
            {
                time_t t = 0;
                time(&t);
                struct tm ttm, nowtm;
                localtime_r(&(st.st_ctime), &ttm);
                localtime_r(&t, &nowtm);
                if (!(ttm.tm_year == nowtm.tm_year &&
                      ttm.tm_mon == nowtm.tm_mon &&
                      ttm.tm_mday == nowtm.tm_mday))
                {
                    //file was not copied today!
                    err.assign("Threshold List file was not copied today");
                    rc = false;
                    break;
                }
            }
        }
    }
    
    return rc;
}

void CPositionManager::RidNegativeBalance()
{
    std::list<CRomMap<tLendingLimit*>*> grplimitlist;
    m_grpToLenderLimit.GetAll(grplimitlist);
    std::list<CRomMap<tLendingLimit*>*>::iterator it = grplimitlist.begin();
    
    pthread_spin_lock(&m_lock);
    
    while (it != grplimitlist.end())
    {
        CRomMap<tLendingLimit*>* pSymbolLimit = *it;
        std::list<tLendingLimit*> limitlist;
        pSymbolLimit->GetAll(limitlist);
        std::list<tLendingLimit*>::iterator itt = limitlist.begin();
        while (itt != limitlist.end())
        {
            tLendingLimit* pLimit = *itt;
            if (pLimit->balance < 0)
            {
                pLimit->balance = 0;
            }
            itt++;  
        }
        it++;
    }
    
    pthread_spin_unlock(&m_lock);
}

void CPositionManager::LoadEasyThresholdList(std::set<void*>& dummy)
{
    CPositionManager::getInstance().LoadThresholdList();
    CPositionManager::getInstance().LoadEasyBorrowList();
}

void CPositionManager::LoadLendingLimits(std::set<void*>& dummy)
{
    CPositionManager::getInstance().LoadlendingLimit();
}


void CPositionManager::ReplaceDotWithSlash(std::string& symbol)
{
    size_t ps = symbol.find_last_of('.');
    
    if (ps != std::string::npos)
    {
        symbol[ps] = '/';
    }
}

void CPositionManager::AppendR(std::string& symbol)
{
    size_t ps = symbol.find_last_of('/');
    if (ps != std::string::npos)
    {
        if (ps == symbol.size() - 2 &&
            symbol[ps + 1] == 'P')
        {
            symbol.append("R");
        }
    }
}

void CPositionManager::ClearPositions()
{
    //m_positions
    std::list<tPosition*> pos;
    m_positions.GetAll(pos);
    std::list<tPosition*>::iterator p_it = pos.begin();
    while (p_it != pos.end())
    {
        delete *p_it;
        p_it++;
    }
    m_positions.RemoveAll();
    
    //m_ClrPositions
    std::list<CRomMap<tPosition*>*> clrpos;
    m_ClrPositions.GetAll(clrpos);
    std::list<CRomMap<tPosition*>*>::iterator clrpos_it = clrpos.begin();
    while (clrpos_it != clrpos.end())
    {
        CRomMap<tPosition*>* pSymPos = *clrpos_it;
        std::list<tPosition*> poss;
        pSymPos->GetAll(poss);
        std::list<tPosition*>::iterator pos_it = poss.begin();
        while (pos_it != poss.end())
        {
            tPosition* pPos = *pos_it;
            delete pPos;
            pos_it++;
        }
        
        delete pSymPos;
        clrpos_it++;
    }
    m_ClrPositions.RemoveAll();
}

void CPositionManager::SetPosRecapFinished() 
{ 
    m_posRecapFinished = true; 
    if (CConfig::getInstance().IsFirstRun() == false)
    {
        //in response to requet sent from Oats
        
        CCsv* pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->SetAt(ROMFIELDS::CSV_COMMAND_FREAK, ROMVALUE::RELOAD_INIT_POSITION);
        pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
        pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Positions for Roning Trading have been reloaded from RDS");
        
        
        std::string sMsg;
        pCsv->GetMsg(sMsg);
        COatsMsg* pMsg = new COatsMsg(sMsg);
        COats::getInstance().AddMessage(pMsg);
        
        CStore<CCsv>::getInstance().Return(pCsv);
    }
}

/*void CPositionManager::UpdatePositionByExecution(const tPosUpdate* pUpdate)
{
    pthread_spin_lock(&m_lock);
    
    CRomMap<long>* pClrPos = NULL;
    
    if (m_PosByExecution.Get(pUpdate->symbol, pClrPos) == false)
    {
        pClrPos = new CRomMap<long>;
        m_PosByExecution->Add(pUpdate->symbol, pClrPos);
    }
    
    long pos = 0;
    if (pClrPos->Get(pUpdate->clract, pos) == false)
    {
        pClrPos->Add(pUpdate->clract, pos);
    }
    
    if (pUpdate->side == ROMVALUE::SIDE_BUY)
    {
        pos += pUpdate->qty_change;
    }
    else
    {
        pos -= pUpdate->qty_change;
    }
   
    pthread_spin_unlock(&m_lock);
}*/

void CPositionManager::LoadPotentialPosFromLiveOrders()
{
    std::list<tPosUpdate*> posUpdates;
    CThreadRomOrdersNew::getInstance().LoadPosUpdates(posUpdates);
    std::list<tPosUpdate*>::iterator it = posUpdates.begin();
    tPosUpdate* update = NULL;
    
    while (it != posUpdates.end())
    {
        update = *it;
        updatePositionOnly(update);
        it++;
    }
}

void CPositionManager::updatePositionOnly(const tPosUpdate* pUpdate)
{
    tPosition* pPos = NULL;
        
    if (m_positions.Get(pUpdate->symbol, pPos) == false)
    {
        pPos = new tPosition;
        pPos->ini_pos = 0;
        pPos->cur_pos = 0;
        m_positions.Add(pUpdate->symbol, pPos);
    }
    
    if (ROMVALUE::SIDE_BUY == pUpdate->side)
    {
        pPos->cur_pos += pUpdate->qty_change;
    }
    else
    {
        pPos->cur_pos -= pUpdate->qty_change;
    }
    
    if (pUpdate->clract.empty() == false)
    {
        CRomMap<tPosition*>* pClrPos = NULL;
        if (m_ClrPositions.Get(pUpdate->clract, pClrPos) == false)
        {
            pClrPos = new CRomMap<tPosition*>;
            m_ClrPositions.Add(pUpdate->clract, pClrPos);
        }

        if (pClrPos->Get(pUpdate->symbol, pPos) == false)
        {
            pPos = new tPosition;
            pPos->ini_pos = 0;
            pPos->cur_pos = 0;
            pClrPos->Add(pUpdate->symbol, pPos);
        }

        if (ROMVALUE::SIDE_BUY == pUpdate->side)
        {
            pPos->cur_pos += pUpdate->qty_change;
        }
        else
        {
            pPos->cur_pos -= pUpdate->qty_change;
        }
    }
}