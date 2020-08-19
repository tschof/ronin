#ifndef _POSITIONMANAGER_H__
#define _POSITIONMANAGER_H__

#include "RdsConnection.h"
#include "Csv.h"
#include "RomMapN.h"
#include "romtimer.h"
#include "RomOrderNew.h"
#include "DBExecutor.h"

/*typedef struct
{
    std::string symbol;
    std::string clract;
    long qty_change;
    int side;
} tPosUpdate;*/

typedef struct
{
    long limit;
    long balance;
} tLendingLimit;

typedef struct
{
    long cur_pos;
    long ini_pos;
} tPosition;

typedef struct
{
    std::string symbol;
    std::string clrs;
    tLendingLimit limit;
} tLendingLimitDetail;

typedef struct
{
    std::string grp;
    std::vector<std::string> symbols;
} tGrpSymbols;

class CPositionManager
{
public:
    //CPositionManager();
    ~CPositionManager();
    
    static CPositionManager & getInstance()
    {
        static CPositionManager theObj;
        return theObj;
    }
    
    static void LogonCallback_RDS(con_obj* co, int is_loggedon, char* log, int len);
    static void MessageCallback_RDS(struct message* m, void* ob);
    static void DummyLowActivityCallback(con_obj* obj) {}
    static bool ReloadEasyBorrowList(time_t now, void* obj);
    static bool ReloadThresholdList(time_t now, void* obj);
    static bool RDSConCheck(time_t now, void* obj);
    static void LoadEasyThresholdList(std::set<void*>& dummy);
    static void LoadLendingLimits(std::set<void*>& dummy);
    
    //bool Ready() { return true; }// for testing without rds //{ return m_ready; }
    void ProcessPosUpdateFromRDS(const CCsv& csv);  
    void ProcessRdsLogon();
    void ProcessRdsDisconnect();
    bool FindClr(const std::string& clr);
    void LoadEasyBorrowList();
    //process sell/short order; 
    bool ProcessSSOrder(CRomOrderNew* pOrder, std::string& err); 
    //process replace of sell/short orders
    bool ProcessSSReplace(CRomOrderNew* pOrder, std::string& err);
    void ProcessSSReplaceReject(CRomOrderNew* pOrder);
    void ProcessPositionUpdate(const tPosUpdate* pUpdate); //from order update, excluding fills of sell/short!!!
    void UpdatePositionOnly(const tPosUpdate* pUpdate);
    //void UpdatePositionByExecution(const tPosUpdate* pUpdate);
    
    void UpdateLendingBalanceOnly(const tPosUpdate* pUpdate);
    
    //bool ReloadEasyBorrowList(std::string& err);
    void Start();
    void HBT_RDS();
    
    void RDSConCheck();
    void ReloadPositionFromRDS();
    bool GetPos(const std::string& symbol, const std::string& clrAcct, tPosition& pos);
    bool GetLenderGroupLimits(const std::string& grp, const std::string& symbol, std::string& clrs, tLendingLimit& limit);
    bool GetClrLenderLimits(const std::string& clr, const std::string& symbol, tLendingLimit& limit);
    bool GetLenderGroupAllLimits(const std::string& grp, std::vector<tLendingLimitDetail>& detail);
    bool AddEasySymbol(const std::string& symbol, bool add_remove);
    bool AddThresholdSymbol(const std::string& symbol, bool add_remove);
    bool LoadlendingLimit();
    bool LoadlendingLimit_test();
    bool FoundEasySymbol(const std::string& symbol);
    bool FoundThresholdSymbol(const std::string& symbol);
    void GetLendingGroups(std::string& grps);
    bool AddLendingGroupLimit(const std::string& grp, const std::string& symbol, const std::string& clrs, long newlimit, std::string& echo);
    bool UpdateLendingGroupLimit(const std::string& grp, const std::string& symbol, long newlimit, std::string& echo);
    
    void LoadThresholdList();
    bool LoadThresholdList_test();
    bool LoadEasyBorrowList_today(std::string& err);
    bool LoadThresholdList_current(std::string& err);
    void RidNegativeBalance();
    void GetGrpSymbols(std::vector<tGrpSymbols>& vgrpsymbols);
    bool PosRecapFinished() { return m_posRecapFinished;};
    void SetRdsMsgTime(const std::string& value) { m_rdsMsgTime = value;}
    void SetPosRecapFinished();
    void LoadPotentialPosFromLiveOrders();

private:
    CPositionManager();
    void loadPositionFromRDS();
    bool ProcessSSAdd(tPosUpdate& tl, std::string& err);
    bool ProcessSSAddNew(tPosUpdate& tl, std::string& err);
    bool LoadClearingAccounts();
    void LoadEasyBorrowList_test();
    bool LoadEasyBorrowList_past();
    bool LoadThresholdList_past();
    bool GetGrpToLenderLimit_test();
    bool GetClrToGrp_test(CRomMap<std::string>& clrToGrp);
    bool loadEasyBorrowList(const char* fpath, std::string& err);
    bool loadThresholdList(const char* fpath, std::string& err);  
    void Connect();
    void LenderUpdateToAdmin(const std::string& lender, const tLendingLimit& limit);
    bool GetClrsFromGrp(std::set<std::string>& clrs, const std::string& grp, std::string& err);
    void ClearEasyBorrowList();
    void ClearThresholdList();
    //void LoadClrToBillingGrp();
    bool Lending(tPosUpdate& tl, std::string& err);
    void ClearLgrp2clrs();
    void GetSymbolsFromGrp(const std::string& grp, std::vector<std::string>& symbols);
    
    void ReplaceDotWithSlash(std::string& symbol);
    void AppendR(std::string& symbol);
    void SendRdsNSeq();
    void ClearPositions();
    //void LoadPotentialPosFromLiveOrders();
    void updatePositionOnly(const tPosUpdate* pUpdate);
    void updateLendingBalanceOnly(const tPosUpdate* pUpdate);
    void ResetLendingBalance(const tPosUpdate* pUpdate);
    
    CRomMap<tPosition*> m_positions;
    CRomMap<CRomMap<tPosition*>*> m_ClrPositions; //no lock needed because update is always
                                             //from the same rds connection
    CRomMap<CRomMap<tLendingLimit*>*> m_clrToLenderLimit;
    CRomMap<CRomMap<tLendingLimit*>*> m_grpToLenderLimit;
    CRomMap<bool> m_easyborrowlist;  //turn on the map lock???
    CRomMap<bool> m_thresholdlist;   //buy fill does not return to lender immediately!!
    CRomMap<bool> m_clrsMap;
    //CRomMap<std::string> m_clr2grp;
    //CRomMap<std::string> m_grp2clrs;
    CRomMap<std::vector<std::string>*> m_lgrp2clrs;
    std::string m_clrs;
    CRdsConnection* m_rdsCon;
    struct async_parse_args* m_AsyncArgsClient;
    time_t m_eswTime;     //the next easy symbol loading failure warning time;
    time_t m_tswTime;     //the next threshold symbol loading failure warning time;
    pthread_spinlock_t m_lock;  //locking m_clrToLenderLimit 
                                //together with m_grpToLenderLimit
                                //and m_positions
    
    CDBExecutor m_rcdb;
    CRomTimer* m_pLoadLendingLimitTimer;
    CRomTimer* m_pLoadEasyThresholdListTimer;
    //CRomMap<CRomMap<long>*> m_PosByExecution;
    //unsigned long m_nCnt;
    std::string m_rdsMsgTime;
    bool m_posRecapFinished;
};



#endif //_POSITIONMANAGER_H__

