#ifndef __DESTSMANAGER_H__
#define __DESTSMANAGER_H__


#include "RomMapN.h"
#include "Thread.h"
#include "DestRequest.h"
#include "HostConnection.h"
#include "ROMTypedef.h"
#include "dart_connection_object.h"
#include "ThreadSet.h"
#include "Utility.h"
#include "romtimer.h"
#include <set>


class CDestsManager : public CThread<CDestRequest*>
{
public:

    typedef std::deque<CDestRequest*> tReqList;
    typedef tReqList::iterator tReqListIt;

    ~CDestsManager();

    static CDestsManager & getInstance()
    {
        static CDestsManager theManager;
        return theManager;
    }

    static void MessageCallback(struct message* m, void* ob);
    static void ConnectCallback(con_obj* co, int is_connected,char* log,
                                int len);
    static void DummyLowActivityCallback(con_obj* obj) {}

    static bool ReconnectCallback(time_t now, void* obj);
    static void SessionEndBlockCallback(std::set<void*>& objs);
    static void SessionStartUnBlockCallback(std::set<void*>& objs);
    static void* ReloadDestsFromDB(void* arg = NULL);
    static void RejectCallBack(std::list<struct message*>& msgs);
    static void* CancelAllDayOrders(void* pArg);

    virtual void Init();

    virtual bool ProcessMessage(tReqList& reqlist);

    void Disconnect(const std::string& name);
    void Disconnected(const std::string& id);
    void Connected(const std::string& id);
    void LoggedOn(const std::string& id);
    void Block(const std::string& id);
    void UnBlock(const std::string& id);
    void ReloadIpPort(const std::string& id);
    void QueryDest(const std::string& id);
    void QueryDests();
    //static void ReloadDests();

    //Sending will be done through different threads
    bool GetConnection(const std::string& id, CHostConnection*& con);
    bool Send(const std::string& id, CCsv& msg, std::string& err);
    bool Send(const std::string& id, char cmd, char* buff, int len, std::string& err);

    bool GetRouteIdFromDestId(const std::string& destID, std::string& routeID);
    bool GetSecTypeFromRouteId(const std::string& routeID, char& secType);
    bool GetDestStatus(const std::string& routeID, ROMTYPEDEF::tConnectionStatus& conStatus);
    bool GetDestNameFromDestId(const std::string& destID, std::string& destName);
    bool GetDestIdFromDestName(const std::string& destName, std::string& destID);
    bool UpdateDestRoute(const std::string& destID, const std::string& newRouteID);
    std::string GetDestRoutes();
    void GetAllConnections(std::list<CHostConnection*>& conList);
    bool Connect(const std::string& id);

    bool LoadDestsFromDB(bool reload, const std::string* pDest = NULL);
    //bool LoadDestFromDB(const std::string* pDest);
    void Reject(const struct message* pMsg);
    void CancelAllDayOrders();
    void UpdateExchangeStatus(const CCsv* pCsv);

    //void RemoveBlockingTimer(const std::string& key);
    //void RemoveUnBlockingTimer(const std::string& key);
    void AddBlockingTimer(const std::string& key, CRomTimer* pTimer);
    bool AddBlockingTimerArg(const std::string& key, void* pObj);
    bool RemoveBlockingTimerArg(const std::string& key, void* pObj);
    void AddUnBlockingTimer(const std::string& key, CRomTimer* pTimer);
    bool AddUnBlockingTimerArg(const std::string& key, void* pObj);
    bool RemoveUnBlockingTimerArg(const std::string& key, void* pObj);

    bool GetDestSC(const std::string& destID, tSC& destSC);
    bool IsOptionOnFuture(int destID, char secType);
    //CHostConnection* GetConnectionFromDestID(const std::string& id);
    
private:
    CDestsManager();
    CHostConnection* GetConnectedDest(const std::string& id);
    CHostConnection* GetDisconnectedDest(const std::string& id);

    bool Connect(CHostConnection* pCon);

    void ProcessReconnect();
    //void ProcessSessionEndCheck(unsigned short now);
    void ProcessBlock(const std::string& id, bool action);
    void ProcessConnected(const std::string& id);
    void ProcessLoggedOn(const std::string& id, time_t lastLogonTime);
    void ProcessDisconnected(const std::string& id);
    void ProcessQueryDest(const std::string& id);
    void ProcessQueryDests();
    bool ProcessReloadIpPort(const std::string& id);
    void ClearMaps();
    void LoadRlogonDestIDs();
    //void DestroyRlogonDestIDs();
    void LoadDestConInfo();

    //All inputs to the following two functions are in central time
    //since they are set in DB in central time!!!
    //unsigned long SecondsToBlock(unsigned short startTime, unsigned short closeTime, unsigned short endWDay);
    //unsigned long SecondsToUnBlock(unsigned short startTime, unsigned short resetWDay);
    unsigned long SecondsToBlock(CHostConnection& con);
    unsigned long SecondsToUnBlock(CHostConnection& con);


    void CleaningTimer();
    void LoadOfDests();

    CRomMap<CHostConnection*> m_ConMap, m_UnConMap;
    CRomMap<std::string> m_DestIdToRouteId;
    CRomMap<std::string> m_DestNameToDestId;
    CRomMap<char> m_RouteIdToSecType;
    CRomMap<tServerConInfo> m_DestConInfo;

    pthread_rwlock_t m_lock;

    CRomMap<CRomTimer*> m_blockingTimerMap;  //timer for session end blocking
    CRomMap<CRomTimer*> m_unblockingTimerMap;  //timer for session open unblocking
    CThreadSet<CRomTimer*> m_garbTimers;
    CThreadSet<int> m_ofDests;   //OptionOnFuture/future logical dest
    //CThreadSet<int> m_optDests;   //Option logical dest
    pthread_mutex_t m_blocklock, m_unblocklock;
};

#endif //__DESTSMANAGER_H__
