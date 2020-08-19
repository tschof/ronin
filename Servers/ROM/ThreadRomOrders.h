#ifndef _ROMORDERSNEW_H__
#define _ROMORDERSNEW_H__

#ifdef _WIN32
#include "windows.h"
#else
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#include <string>
#include "Config.h"
#include "RomOrder.h"
#include "OatsMsg.h"
#include "ThreadOrder.h"
#include "Queue.h"
#include "ThreadList.h"
#include "ThreadSet.h"
#include "ThreadOrder.h"
#include "RomCount.h"
#include "ClientProfileManager.h"
#include "RomCancel.h"


class CThreadRomOrders
{
public:

    typedef CThreadSet<CThreadOrder*> tOrderSet;

    CThreadRomOrders();
    ~CThreadRomOrders();

    void StoreOrder(CRomOrder* pOrder);
    //void DestroyOrder(CRomOrder* pOrder);
    //void BuildCancels(CCsv& filter, std::deque<COatsMsg*>& oatsMsgs, std::set<CCsv*>& cancels, bool dayorderonly = false);
    void BuildCancels(CCsv& filter, std::set<CCsv*>& cancels, bool dayorderonly = false);

    //void BuildDestCancels(const std::string& destID, std::deque<COatsMsg*>& oatsMsgs, std::set<CCsv*>& cancels);
    void BuildDestCancels(const std::string& destID, std::set<CCsv*>& cancels);
    //void BuildAllCancels(std::deque<COatsMsg*>& oatsMsgs, std::set<CRomCancel*>& cancels);

    void UpdateClientTag(const char* oldtag, const char* newtag);
    bool GetOrderFromRomTag(const char* tag, CRomOrder* pOrder);
    bool GetOrderFromClientTag(const std::string& tag, CRomOrder* pOrder);

    bool GetOrderFromRomTagCancel(const char* tag,
                                  CRomOrder* pOrder,
                                  std::string& err,
                                  bool ownerCancel,
                                  CThreadOrderHandler& tHD,
                                  CCsv* pCancel);

    bool GetOrderFromClientTagCancel(const std::string& tag,
                                     const std::string& actTa,
                                     CRomOrder* pOrder,
                                     std::string& err,
                                     bool ownerCancel,
                                     CThreadOrderHandler& tHD,
                                     CCsv* pCancel);

    bool GetOrderFromClientTagReplace(const std::string& tag, CCsv& msg, CRomOrder* pOrder, std::string& err);
    bool UpdateOrder(const std::string& romTag, CCsv& msg, CRomOrder* pOrder, std::string& err);
    bool GetNextRomExecTag(const std::string& romTag, std::string& romExecTag);
    void GetNextRomTag(const std::string& owner, std::string& romTag);
    //bool GetNextRomExTag(const std::string& romTag, std::string& romExTag);
    void SetStatus(const std::string& romTag, short status);
    void SetCancelPending(const std::string& romTag, bool value);
    void SetReplacePending(const std::string& romTag, bool value);
    void TestSize();
    bool Filter(const CRomOrder& order, CCsv& filter);
    void ResetReplacingFields(const std::string& romTag);
    bool FillParent(const std::string& parentRomTag, long lastShares, tBalanceUpdate& BUpdate, CRomOrder* pParent);
    void UpdateLegsReplace(const std::string& owner, const std::vector<CCsv*>& legs, long shareChange);

    CRomOrder* RemoveOrder(const std::string& romTag);
    void StoreLiveOrders(const std::set<databuf_t*>& bufs);
    void UpdateBalanceFromLiveOrder(const CRomOrder& order);

private:
    void CleanUp();
    //bool AllocateMemoryMap();
    void ExpandFreeList();
    void BuildBalanceUpdateFromOrder(const CRomOrder& order, tBalanceUpdate& update);
    bool LoadOrdersFromDB();
    void ClearMaps();
    void InitOrderNum();
    //void destroyorder(CRomOrder* pOrder);
    //void destroyorder(CThreadOrder* pOrder);
    CRomOrder* removeOrder(const std::string& romTag);
    void MapOrder(CThreadOrder* ptOrder, CRomOrder* pOrder);
    void SetRatio(const std::string& romTag, long value);
    bool SetDateInstanceStr();

    CRomMap<CThreadOrder*> m_RomTagToOrder;
    CRomMap<CThreadOrder*> m_ClientTagToOrder;
    CRomMap<tOrderSet*> m_OwnerToOrderSet;
    CRomMap<tOrderSet*> m_DestToOrderSet;

    unsigned short m_mDay;

    CThreadQueue<CThreadOrder*> m_FreeList;
    std::set<void*> m_OrderSets;
    CRomCount m_OrderNum;
    std::string m_DateInstanceStr;
    pthread_rwlock_t m_lock; //lock for memory map expansion

};

#endif
