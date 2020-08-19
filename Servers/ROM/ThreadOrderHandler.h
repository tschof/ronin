#ifndef _THREADORDERHANDLER_H__
#define _THREADORDERHANDLER_H__

#include "Csv.h"
#include "ClientProfileManager.h"
#include "ThreadRomOrders.h"
#include "Oats.h"

class CThreadOrderHandler
{
public:

    typedef std::list<CCsv*> tReqList;
    typedef tReqList::iterator tReqListIt;

	CThreadOrderHandler();
	~CThreadOrderHandler();

    static CThreadOrderHandler & getInstance()
	{
        static CThreadOrderHandler theHandler;
        return theHandler;
	}

    static void ProcessAdminCancels(CCsv* pArg)
    {
        CThreadOrderHandler::getInstance().ProcessClientCancels(pArg);
    }

    static void* ProcessAdminCANCELS(void* pArg)
    {
        CCsv* pCsv = (CCsv*)pArg;
        CThreadOrderHandler::getInstance().ProcessClientCancels(pCsv);
        if (pCsv)
        {
            CCsvStore::getInstance().Return(pCsv);
        //delete pCsv;
        }
        return NULL;
    }

	//from client
	bool PassThroughToExchange(CCsv* pCsv);
	bool ProcessSingleOrder(CCsv* pCsv);
	bool ProcessUserDefinedSpreadOrder(CCsv* pBody, std::vector<CCsv*>* pLegs);
	bool ProcessClientCancel(CCsv* pCsv);
	bool ProcessClientCancels(CCsv* pCsv, bool dayorderonly = false);
    bool ProcessDestCancels(const std::string& destID);
	bool ProcessClientReplace(CCsv* pCsv);
        bool ProcessClientHeartBeat(CCsv* pCsv);
  //bool ProcessAdminZap(CCsv* pMsg);

	//from dest
	bool ProcessHostUpdate(CCsv* pCsv);
    bool ProcessHostLegUpdate_ProductSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData);
    bool ProcessHostSingleUpdate(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData);
    bool ProcessHostContainerUpdate_ProductSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData);
    bool ProcessHostContainerUpdate_UserDefinedSpread(CCsv* pCsv, CRomOrder* pOrder, tDropInfo* pData);

    inline void StoreOrder(CRomOrder* pOrder)
    {
        m_pOrders->StoreOrder(pOrder);
    }

    void StoreLiveOrders(const std::set<databuf_t*>& bufs)
    {
        m_pOrders->StoreLiveOrders(bufs);
    }

    void DoRomCancelPending(CCsv* pCsv, CRomOrder* pOrder);

private:
    bool ValidateReplace(CCsv& replace, CRomOrder* pOrder, tBalanceUpdate* pBUpdate, std::string& error);
    bool ValidateCancel(CCsv* pCancel, CRomOrder* pOrder, std::string& error);
    bool ValidateOrder(const CRomOrder& order, const std::vector<CCsv*>* pLegs, tBalanceUpdate& BUpdate, std::string& error);
    void MakeOrderReject(CCsv& msg, const std::string& reason);
    void MakeOrderReject(CCsv& msg, const CRomOrder& order, const std::string& reason);
	void MakeCancelReject(CCsv& msg, const std::string& reason);
	void MakeReplaceReject(CCsv& msg, const std::string& reason);
	void ReplaceAddRequiredFields(CCsv& msg, const CRomOrder& Order);
	void CancelAddRequiredFields(CCsv& msg, const CRomOrder& Order);

    void DoOats(COats::tMsgList& msgs);
    void DoOats(COatsMsg* msg);
  //bool ValidateCapacity(const CRomOrder* pOrder, std::string& err);
    void DoDropCopy(CCsv& msg, CRomOrder* pOrder, tDropInfo* pData);
    void UpdateTraderBalance(const CCsv& msg, CRomOrder* pOrder);
    std::string GetDropCopyText(const CRomOrder* pOrder, CCsv& msg);
    void UpdateLegsReplace(const std::vector<CCsv*>& legs, long shareChange);
    void MakeBodyFill(CRomOrder* pOrder, long lastShare, const std::string& omTime);
	CThreadRomOrders* m_pOrders;
	CRomMap<tBalanceUpdate> m_ReplacingDelts;
};

#endif //_THREADORDERHANDLER_H__
