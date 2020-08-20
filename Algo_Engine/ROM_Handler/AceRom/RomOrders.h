#ifndef _ROMORDERS_H__
#define _ROMORDERS_H__



#include <hash_map>
#include <string>
#include "Config.h"
#include "ROMMessage.h"
#include "RomOrder.h"
#include "ace\Mem_Map.h"


typedef struct
{
	CRomOrder	Orders[1];
}OrdersMemoryStruct;

class CRomOrders
{
public:
	typedef stdext::hash_map<std::string, CRomOrder*> tTagOrderMap;
	typedef tTagOrderMap::iterator tTagOrderMapIterator;
	typedef std::set<CRomOrder*> tOrderList;
	typedef tOrderList::iterator tOrderListIterator;
	typedef stdext::hash_map<std::string, tOrderList*> tOwnerOrderListMap;
	typedef tOwnerOrderListMap::iterator tOwnerOrderListMapIterator;
	typedef stdext::hash_map<short, tOrderList*> tDestOrderListMap;
	typedef tDestOrderListMap::iterator tDestOrderListMapIterator;
	typedef std::list<CRomOrder*> tOrderMapList;

	CRomOrders(unsigned short hdID, bool loadFromDB);
	~CRomOrders();

	void StoreOrder(CRomOrder* pOrder);
	void DestroyOrder(CRomOrder* pOrder);
	void DestroyOrderByRomTag(const char* tag);
	void DestroyOrderByClientTag(const char* tag);
	void CancelClientAll(const char* owner, short dest = -1);
    void CancelDestAll(short dest, bool cancelGTDC = true);
	void Cancels(const char* owner, short dest);
	void Cancels(CRomCsv& filter, bool fromAdmin);
	void CancelAll();
    void UpdateClientTag(const char* oldtag, const char* newtag, const char* owner);

	CRomOrder* FindOrderFromRomTag(const char* tag);
	CRomOrder* FindOrderFromClientTag(const std::string& tag);
    //std::string NextRomTag(const char* owner);
	void GetLiveOrders(const std::string& login, std::list<std::string>& clientQ);
	CRomOrder* CreateOrder(CRomCsv& csv);
	void TestSize();
	void ProcessGapFill(short destID, unsigned long newLastSent);
	bool Filter(const CRomOrder& order, CRomCsv& filter, bool fromAdmin);

private:
	//bool LoadFromMap();
    //bool AllocateMemoryMap(unsigned long numOrders);
	
	void RemoveFromMaps(CRomOrder* pOrder);
	void ClearMaps();
	CRomOrder* GetOrderSlot();

	DWORD GetSavedRomOrderId();
	
	void SpreadOrderPostSet(CRomOrder* pOrder);
	void CrossOrderPostSet(CRomOrder* pOrder);
	void BasketOrderPostSet(CRomOrder* pOrder);

	void CleanUp();
	void DestroyOrders(std::vector<CRomOrder*>& orders);
	bool MapFileExist();

	void GetMapFileName(unsigned short hdID);
	void InitMap();
	void LoadMap();
	bool AllocateMemoryMap();

	tTagOrderMap m_RomTagToOrder;
	tTagOrderMap m_ClientTagToOrder;
	tOwnerOrderListMap m_OwnerToOrderList;
    tDestOrderListMap m_DestToOrderList;

	unsigned short m_mDay;


	ACE_Mem_Map			m_MemMap;
	std::string			m_MapFileName;
	tOrderMapList		m_FreeList;
	OrdersMemoryStruct*	m_pMemoryStruct;
	unsigned long		m_AllocateStep;
	unsigned long		m_nAllocatedOrders;
};


#endif //_ROMORDERS_H__