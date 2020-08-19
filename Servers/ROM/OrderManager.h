#ifndef __ORDERMANAGER_H__
#define __ORDERMANAGER_H__

#include <string>
//#include "RomMap.h"
#include "RomMapN.h"
#include "Csv.h"
#include "romvaluedefs.h"
#include "Config.h"
#include "OrderHandler.h"
#include "RomCount.h"

class COrderManager
{
public:
	COrderManager();
	~COrderManager();

	static COrderManager & getInstance()
	{
    static COrderManager theManager;
    return theManager;
	}

  void CancelDestOrders(const std::string& destID);

	void AddClientRequest(CCsv* pCsv); //messages from client, called by ClientHandler, from ClientsManager proactor
  bool AddHostUpdate(CCsv* pCsv);
	bool AddApprovedOrder(CCsv* pCsv); //message from ClientProfileManager, retrieved from thread queue
	bool ReturnNotApprovedOrder(CCsv* pCsv);

  //void UpdateAllDestRoutes(CRomMap<unsigned short, unsigned short>& newMap);

  void RemoveClientTag(const std::string& tag);
  void UpdateClientTag(const std::string& oldtag, const std::string& newtag);
  void AddOrder(CRomOrder* pOrder);
  void AddToClientTagToHIDMap(CRomOrder* pOrder);
  void Init();

private:

	unsigned long long GetNextOrderNum();
	bool GetOrderNumFromRomTag(const std::string& tag, unsigned long long& orderNum);
	bool GetHIDThroughClientTag(const std::string& tag, unsigned short& hID);
	unsigned long long GetNextRomTag(const std::string& owner, std::string& romTag);
  unsigned long long GetNextLegRomTag(const std::string& parentRomTag, std::string& romTag);


	//void LoadOrdersFromDB();
  bool LoadOrdersFromDB_New();

	bool AddOrderRequest(CCsv* pCsv);
	bool AddReplaceOrCancelOrZap(CCsv* pCsv);
	bool AddCancels(CCsv* pCsv);
	
	void InitOrderNum();
	void InitHandlers();
	
	//CRomMap<std::string, unsigned short> m_ClientTagToHIDMap;
  //CRomMap<unsigned short, unsigned short> m_DestToDestRouteMap;
  CRomMap<unsigned short> m_ClientTagToHIDMap;
  //CRomMap m_DestToDestRouteMap;
  CRomCount m_OrderNum;
  std::vector<COrderHandler*> m_Handlers;
  std::string m_currentDay;
};


#endif //__ORDERMANAGER_H__