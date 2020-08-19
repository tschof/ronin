#ifndef __OATS_H__
#define __OATS_H__

#include "Thread.h"
#include "OatsMsg.h"
#include "Connection.h"
#include "HostConnection.h"
#include "ClientConnection.h"
#include "dart_connection_object.h"

class COats : public CThread<COatsMsg*>
{
public:

    typedef std::deque<COatsMsg*> tMsgList;
    typedef tMsgList::iterator tMsgListIt;

    static COats & getInstance()
    {
        static COats theOats;
        return theOats;
    }

    static void MessageCallback(struct message* m, void* ob);
    static void ConnectCallback(con_obj* co, int is_loggedon,char* log, int len);
    static void DummyLowActivityCallback(con_obj* obj) {}

    static bool ReconnectCallback(time_t now, void* obj);

    virtual void Init();
    void ReConnect();
    void Connected();
    void LoggedOn();
    void Disconnected();
    void ProcessMessage(char* data,  unsigned long len);
    void HostConnectionUpdate(CHostConnection& con, std::string adminID);
    void ClientConnectionUpdate(CClientConnection& con, std::string adminID);
    void Send(const std::string& msg);
    //void ExchangeConnectionUpdate(CHostConnection& con, std::string adminID);
    void SetOrderQueryFinished(bool value) { m_OrderQueryFinished = value; }
    bool OrderQueryFinished() { return m_OrderQueryFinished; }
    bool CreateNewEffective() { return m_CreateNewEffective; }
    void SetCreateNewEffective(bool value) { m_CreateNewEffective = value; }
    time_t GetLastLogonTime();
    void ReloadIpPort();

    ~COats();

    virtual bool ProcessMessage(tMsgList& msglist);
    bool Send(const char* pBuf, int len);

private:
    COats();
    void ProcessAdminQueryOrderDest(CCsv& msg);
    void ProcessAdminModifyOrderDestAccess(CCsv& msg);
    void ProcessAdminQueryTraderStatus(CCsv& msg);
    void ProcessAdminDisconnectUser(CCsv& msg);
    void ProcessAdminReloadTraders(CCsv& msg);
    void ProcessAdminReloadOrderDests(CCsv& msg);
    void ProcessAdminExemptTraderLimits(CCsv& msg);
    void ProcessAdminExemptTraderMsgRate(CCsv& msg);
    void ProcessAdminQueryTraderLogons(CCsv& msg);
    void ProcessAdminReloadDestIpPort(CCsv& msg);
    void ProcessAdminDestRouteUpdate(CCsv& msg);
    void ProcessAdminQueryDestRoutes(CCsv& msg);
    void ProcessAdminHostConnectStatusQuery(CCsv& msg);
    void ProcessAdminClientConnectStatusQuery(CCsv& msg);
    void ProcessAdminDisconnectHost(CCsv& msg);
    void ProcessAdminBlockUser(CCsv& msg);
    void ProcessDestDisconnectRom(CCsv& msg);
    void ProcessAdminText(CCsv& msg);
    bool ProcessAdminCancel(CCsv* pMsg);
    void ProcessAdminCancels(CCsv* pMsg);
    bool ProcessAdminStatus(CCsv* pMsg);
    bool ProcessAdminZap(CCsv* pMsg);
    void ProcessAdminQueryClearingActSecLimitsStatus(CCsv& msg);
    void ProcessAdminReloadClearingLimits(CCsv& msg);
    void ProcessAdminExemptPriceCheck(CCsv& msg);
    void ProcessAdminQueryBillingGrpLimits(CCsv& msg);
    void ProcessAdminReloadBillingGrpLimits(CCsv& msg);
    
    //To PositionManager
    void ProcessAdminReloadLenderLimits(CCsv& msg);
    void ProcessAdminReloadEasyBorrowList(CCsv& msg);
    void ProcessAdminReloadInitPosition(CCsv& msg);
    void ProcessAdminQueryLenderLimits(CCsv& msg);
    void ProcessAdminClrLendingLimitQuery(CCsv& msg);
    void ProcessAdminUpdateLenderLimit(CCsv& msg);
    void ProcessAdminQueryPosition(CCsv& msg);
    void ProcessAdminUpdatePosition(CCsv& msg);
    void ProcessAdminAddEasySymbol(CCsv& msg);
    void ProcessAdminQueryEasySymbol(CCsv& msg);
    //void ProcessAdminGetLendingGroups(CCsv& msg);
    void ProcessAdminGetLendingGroupsSymbols(CCsv& msg);
    void ProcessAdminReloadThresholdList(CCsv& msg);
    void ProcessAdminQueryThresholdSymbol(CCsv& msg);
    void ProcessAdminAddThresholdSymbol(CCsv& msg);
    void ProcessAdminExemptClrActLimits(CCsv& msg);
    void ProcessAdminExemptBgpLimits(CCsv& msg);
    void ProcessAdminReloadFuture(CCsv& msg);
    void ProcessAdminQueryFuture(CCsv& msg);
    bool Connect();
    void BuildOatsMsg(databuf_t* pBuffer, COatsMsg* pOatsMsg);
    void CleanQueryResult();
    void PushStats();
    //void PushRecordLogon();

    CHostConnection* m_pCon;
    struct init_args m_args;
    std::set<databuf_t*> m_queryResult;
    //std::deque<databuf_t*> m_recordLogons;

    bool m_OrderQueryFinished;
    bool m_CreateNewEffective;
};

#endif //__OATS_H__
