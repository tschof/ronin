#ifndef __ORDERMANAGER_H__
#define __ORDERMANAGER_H__

#include <string>
#include "ace/Task_T.h"
#include "ace/Hash_Map_Manager_T.h"
#include "RomCsv.h"
#include "romvaluedefs.h"
#include "Config.h"
#include "OrderHandler.h"

static const int HANDLERS_SIZE = 5;

class COrderManager
{
public:
	COrderManager();
	~COrderManager();

	typedef ACE_Hash_Map_Manager_Ex<const char*, 
									ACE_UINT64, 
									ACE_Hash<const char*>, 
									ACE_Equal_To<const char*>,
									ACE_SYNCH_MUTEX> tStringToUINT63Map;

	static COrderManager & getInstance()
	{
		static COrderManager theManager;
		return theManager;
	}

	bool AddClientRequest(CRomCsv* pCsv); //messages from client, called from ClientHandler
	bool AddHostUpdate(CRomCsv* pCsv); //messages from exchange, called from DestHandler
	bool AddApprovedOrder(CRomCsv* pCsv); //message from ClientProfileManager
	
private:

	ACE_UINT64 GetNextOrderNum();
	ACE_UINT64 GetOrderNumFromRomTag(const std::string& tag);
	ACE_UINT64 GetOrderNumThroughClientTag(const std::string& tag);
	void GetNextRomTag(const std::string& owner, std::string& tag);

	void LoadOrdersFromDB();
	bool AddOrderRequest(CRomCsv* pCsv);
	bool AddReplaceOrCancel(CRomCsv* pCsv);
	bool AddCancels(CRomCsv* pCsv);

	void InitOrderNum();
	void InitHandlers();

	tStringToUINT63Map m_ClientTagToRomOrderNumMap;

	ACE_UINT64 m_OrderNum;
	COrderHandler* m_pHandlers[HANDLERS_SIZE];
};


#endif //__ORDERMANAGER_H__