#ifndef __ORDERHANDLER_H__
#define __ORDERHANDLER_H__

#include "thread.h"
#include "RomOrders.h"
#include "Oats.h"
#include "ClientProfileManager.h"

class COrderHandler : public CThread<CCsv*>
{
public:
  
  typedef std::list<CCsv*> tReqList;
  typedef tReqList::iterator tReqListIt;

	COrderHandler(unsigned short id, bool loadOrders);
	~COrderHandler();

  virtual bool ProcessMessage(tReqList& reqlist);


	void CleanUp();

	//This is the interface used by COrderManager to install 
	//the current live orders retrived from DB.  Since this happens 
	//before any activities the AddOrder is called directly from the 
	//COrderManager thread!!!
	void AddOrder(CRomOrder* pOrder); 

//private:

	//from client
	bool PassThroughToExchange(CCsv* pCsv);
	bool ProcessSingleOrder(CCsv* pCsv);
	bool ProcessSpreadOrder(CCsv* pCsv);
	bool ProcessClientCancel(CCsv* pCsv);
	bool ProcessClientCancels(CCsv* pCsv);
	bool ProcessClientReplace(CCsv* pCsv);


	//from dest
	bool ProcessHostUpdate(CCsv* pCsv);
  bool ProcessHostLegUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData);
  bool ProcessHostSingleUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData);
  bool ProcessHostContainerUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData);

  bool ValidateOrder(const CCsv& order, const std::string& login, std::string& error);
  void MakeOrderReject(CCsv& msg, const std::string& reason);
	void MakeCancelReject(CCsv& msg, const CRomOrder& Order, const std::string& reason);
	void MakeReplaceReject(CCsv& msg, const std::string& reason);
	void ReplaceAddRequiredFields(CCsv& msg, const CRomOrder& Order);
	void CancelAddRequiredFields(CCsv& msg, const CRomOrder& Order);

  void DoOats(COats::tMsgList& msgs);
  void DoOats(COatsMsg* msg);
  bool ValidateCapacity(const CRomOrder* pOrder, std::string& err);
  void DoDropCopy(CCsv& msg, CRomOrder* pOrder, CTraderData* pData);
  void UpdateTraderBalance(const CCsv& msg, CRomOrder* pOrder);//, CTraderData* pData);
  std::string GetDropCopyText(const CRomOrder* pOrder, CCsv& msg);

	CRomOrders* m_pOrders;
};

#endif //__ORDERHANDLER_H__
