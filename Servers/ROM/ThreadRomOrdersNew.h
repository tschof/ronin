#ifndef _THREADROMORDERSNEW_H__
#define _THREADROMORDERSNEW_H__

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
//#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <string>
#include "Config.h"
#include "RomOrderNew.h"
#include "OatsMsg.h"
#include "Queue.h"
#include "ThreadList.h"
#include "ThreadSet.h"
#include "ThreadOrderNew.h"
#include "RomCount.h"
#include "ClientProfileManagerNew.h"
#include "RomCancel.h"
#include "ClientConnection.h"
#include "PositionManager.h"

typedef struct
{
    CRomOrderNew* pOrder;
    CCsv* pCsv;
} tKid;  //for reject purpose!!!

class CThreadRomOrdersNew
{
public:

    static CThreadRomOrdersNew & getInstance()
	{
        static CThreadRomOrdersNew theOrders;
        return theOrders;
	}

    static void* ProcessAdminCANCELS(void* pArg)
    {
        CCsv* pCsv = (CCsv*)pArg;
        CThreadRomOrdersNew::getInstance().ProcessClientCancels(pCsv);
        if (pCsv)
        {
            CStore<CCsv>::getInstance().Return(pCsv);
        }
        return NULL;
    }

    typedef CThreadSet<CThreadOrderNew*> tOrderSet;

    ~CThreadRomOrdersNew();

    CThreadOrderNew* StoreOrder(CRomOrderNew* pOrder);
    void BuildCancels(CCsv& filter, std::set<std::string>& clientTags, bool dayorderonly = false);

    void BuildDestCancels(const std::string& destID, std::set<std::string>& romTags);

    void UpdateClientTag(const std::string& oldtag, const std::string& newtag);
    bool GetOrderFromRomTag(const char* tag, CRomOrderNew* pOrder);
    bool GetOrderFromClientTag(const std::string& tag, CRomOrderNew* pOrder);

    void GetNextRomTag(const std::string& owner, std::string& romTag);

    void SetStatus(const std::string& romTag, short status);
    void SetCancelPending(const std::string& romTag, bool value);
    void SetReplacePending(const std::string& romTag, bool value);
    void TestSize();
    bool Filter(const CRomOrderNew& order, CCsv& filter);
    void ResetReplacingFields(const std::string& romTag);
    //bool FillParent(const std::string& parentRomTag, long lastShares, tBalanceUpdate& BUpdate, CRomOrderNew* pParent);
    void UpdateLegsReplace(const std::string& owner, const std::vector<CCsv*>& legs, long shareChange);

    CRomOrderNew* RemoveOrder(const std::string& romTag);
    void StoreLiveOrders(const std::set<databuf_t*>& bufs);
    void UpdateBalanceFromLiveOrder(const CRomOrderNew& order);
    void UpdateBalanceFromLiveUserDefinedSpreadOrder(CRomOrderNew& order, std::vector<CRomOrderNew*>& legs);
    bool SendOatsMsg(CCsv* pCsv);

    //Order processing
    bool ProcessSingleOrder(CCsv* pCsv, CClientConnection* pCon = NULL);
    bool ProcessUserDefinedSpreadOrder(CCsv* pBody, std::vector<CCsv*>* pLegs, CClientConnection* pCon = NULL);

    bool ProcessClientCancel(CCsv* pCsv, CClientConnection* pCon = NULL);
    bool ProcessClientCancels(CCsv* pCsv, bool dayorderonly = false);
    bool ProcessDestCancels(const std::string& destID);
	bool ProcessClientReplace(CCsv* pCsv, CClientConnection* pCon = NULL);
    bool ProcessHostUpdate(CCsv* pCsv);
    bool LoadProductMultiplierMap();
    void GetFuture(const std::string& product, std::deque<std::string>& futures);
    bool ReloadFutures();
    
    CRomMap<double>& ProductMultiplier() { return m_ProductMultiplier; }
    
    void LoadPosUpdates(std::list<tPosUpdate*>& updates);
    bool LoadOrdersFromDB();

private:
    CThreadRomOrdersNew();
    bool ValidateSendReplace(CCsv* pReplace, CClientConnection* pCon);
    bool ValidateSendCancel(CCsv* pCancel, bool ownercancel, CClientConnection* pCon = NULL);
    bool ValidateOrder(CRomOrderNew& order, CCsv* pCsv, const std::vector<CCsv*>* pLegs, std::vector<tBalanceUpdate*>& bupdates, std::string& error, CClientConnection* pCon);
    void MakeOrderReject(CCsv& msg, const std::string& reason);
    void MakeOrderReject(CCsv& msg, const CRomOrderNew& order, const std::string& reason);
    void MakeCancelReject(CCsv& msg, const std::string& reason);
    void MakeReplaceReject(CCsv& msg, const std::string& reason);

    void CleanUp();
    void ExpandFreeList();
    void BuildBalanceUpdateFromOrder(const CRomOrderNew& order, tBalanceUpdate& update);
    //bool LoadOrdersFromDB();
    void SetOrder(CRomOrderNew* pOrder, queue_t row);
    void ClearMaps();
    void InitOrderNum();

    CRomOrderNew* removeOrder(const std::string& romTag);
    void MapOrder(CThreadOrderNew* ptOrder, CRomOrderNew* pOrder);
    void SetRatio(const std::string& romTag, long value);
    bool SetDateInstanceStr();
    
    void InstallPosFromSSLiveOrderLeave(CRomOrderNew* pOrder);
    void InstallLendingBalanceFromCurrentSessionDeadOrderCum();
    
    void MakeTheClientTag(std::string& theTag, const char* owner, const char* clienttag);
    void LoadSpreadOriginalSide(std::map<std::string, short>& spreadOriginalSide);

    CRomMap<CThreadOrderNew*> m_RomTagToOrder;
    CRomMap<CThreadOrderNew*> m_ClientTagToOrder;
    CRomMap<tOrderSet*> m_OwnerToOrderSet;
    CRomMap<tOrderSet*> m_DestToOrderSet;
    CRomMap<double> m_ProductMultiplier;
    unsigned short m_mDay;

    CThreadQueue<CThreadOrderNew*> m_FreeList;
    std::set<void*> m_OrderSets;
    CRomCount m_OrderNum;
    std::string m_DateInstanceStr;
    CRomMap<tKeyedBalanceUpdate> m_ReplacingDelts;
            
    pthread_rwlock_t m_lock; 

};

#endif  //_THREADROMORDERSNEW_H__
