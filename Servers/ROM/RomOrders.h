#ifndef _ROMORDERS_H__
#define _ROMORDERS_H__


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

typedef struct
{
	CRomOrder		order;
	unsigned long	check;
} ORDER;

typedef struct
{
	ORDER	orders[1];
}OrdersMemoryStruct;

class CRomOrders
{
public:
	
  typedef std::set<CRomOrder*> tOrderSet;
	typedef tOrderSet::iterator tOrderSetIt;
  typedef std::list<CRomOrder*> tOrderList;
  typedef tOrderList::iterator tOrderListIt;

	CRomOrders(unsigned short hdID, bool loadOrder);
	~CRomOrders();

	void AddOrder(CRomOrder* pOrder);

	void StoreOrder(CRomOrder* pOrder);
	void DestroyOrder(CRomOrder* pOrder);
	void DestroyOrderByRomTag(const char* tag);
	void DestroyOrderByClientTag(const char* tag);
  void CancelClientAll(const char* owner, std::string dest = "-1");
  void CancelDestAll(std::string dest, bool cancelGTDC = true);
  void Cancels(CCsv& filter, std::list<COatsMsg*> oatsMsgs);
	void CancelAll();
  void UpdateClientTag(const char* oldtag, const char* newtag, const char* owner);

	CRomOrder* FindOrderFromRomTag(const char* tag);
	CRomOrder* FindOrderFromClientTag(const std::string& tag);
 	void GetLiveOrders(const std::string& login, std::list<std::string>& clientQ);
	CRomOrder* CreateOrder(CCsv& csv);
	void TestSize();
	bool Filter(const CRomOrder& order, CCsv& filter);

private:
  void DeleteMapFile();
	
  bool AllocateMemoryMap(bool realloc);
	bool AllocateMemoryMap_WINDOWS(bool realloc);
  bool AllocateMemoryMap_UNIX(bool realloc);
  bool AllocateMemoryMap_UNIX_New(bool realloc);
  bool ReloadFromNewMap(bool realloc);

	void RemoveFromMaps(CRomOrder* pOrder);
	void ClearMaps();
	CRomOrder* GetOrderSlot();

	void SpreadOrderPostSet(CRomOrder* pOrder);
	void CrossOrderPostSet(CRomOrder* pOrder);
	void BasketOrderPostSet(CRomOrder* pOrder);

	void CleanUp();
	void DestroyOrders(std::vector<CRomOrder*>& orders);

  //CRomMap<std::string, CRomOrder*> m_RomTagToOrder;
	//CRomMap<std::string, CRomOrder*> m_ClientTagToOrder;
	//CRomMap<std::string, tOrderSet*> m_OwnerToOrderSet;
  //CRomMap<std::string, tOrderSet*> m_DestToOrderSet;

  CRomMap<CRomOrder*> m_RomTagToOrder;
	CRomMap<CRomOrder*> m_ClientTagToOrder;
	CRomMap<tOrderSet*> m_OwnerToOrderSet;
  CRomMap<tOrderSet*> m_DestToOrderSet;

	unsigned short m_mDay;

  tOrderSet     m_Orders;
	tOrderList		m_FreeList;

  OrdersMemoryStruct*	m_pMemoryStruct;
  unsigned long m_curSlotsNum;
  unsigned short m_hdID;
  char m_MapFileName[125];

#ifdef _WIN32
  HANDLE				m_hFileHandle;
	HANDLE				m_hFileMap;
	LPVOID				m_pFileView;
#else
  int m_hFileHandle;
#endif

};


#endif //_ROMORDERS_H__