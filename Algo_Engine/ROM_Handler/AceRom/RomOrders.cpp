#include "stdafx.h"
#include "RomOrders.h"
#include "RomCancel.h"
#include "RomReject.h"
#include "ClientProfileManager.h"


CRomOrders::CRomOrders(unsigned short hdID, bool loadFromDB)
{
	m_nAllocatedOrders = 0;
	m_AllocateStep = GetPrivateProfileInt("Option Information","Allocated Orders", 3000, (CConfig::getInstance().datFile()).c_str());

	GetMapFileName(hdID);

	//Loading Orders from DB will be COrderManager's job!!!
	if (CConfig::getInstance().IsFirstRun() ||
		true == loadFromDB) 
	{
		InitMap();
	}
	else
	{
		LoadMap();
	}
}

CRomOrders::~CRomOrders(void)
{
	ClearMaps();
	//CloseReg();
}

void CRomOrders::InitMap()
{
	unsigned int size = sizeof(OrdersMemoryStruct) + (m_AllocateStep - 1) * sizeof(CRomOrder);
	m_MemMap.map( m_MapFileName.c_str(), 
				  size,
				  O_RDWR | O_CREAT,
				  ACE_DEFAULT_FILE_PERMS,
				  PROT_RDWR,
				  ACE_MAP_SHARED);

	m_pMemoryStruct = (OrdersMemoryStruct*)m_MemMap.addr();
	memset(m_pMemoryStruct, 0, size);
	m_nAllocatedOrders = m_AllocateStep + 1;
	CRomOrder* pOrder = NULL;

	for (int i = 0; i < m_nAllocatedOrders; i++)
	{
		pOrder = &((m_pMemoryStruct->Orders)[i]);
		pOrder->SetUsed(false);
		m_FreeList.push_back(pOrder);
	}
}

void CRomOrders::LoadMap()
{
	m_MemMap.map( m_MapFileName.c_str(), 
				  -1,
				  O_RDWR | O_CREAT,
				  ACE_DEFAULT_FILE_PERMS,
				  PROT_RDWR,
				  ACE_MAP_SHARED);

	m_pMemoryStruct = (OrdersMemoryStruct*)m_MemMap.addr();
	m_nAllocatedOrders = (m_MemMap.size() - sizeof(OrdersMemoryStruct) + sizeof(CRomOrder)) / sizeof(CRomOrder);
	CRomOrder* pOrder = NULL;
	
	for (int i = 0; i < m_nAllocatedOrders; i++)
	{
		pOrder = &((m_pMemoryStruct->Orders)[i]);
		if (pOrder->Used())
		{
			StoreOrder(pOrder);
			CClientProfileManager::getInstance().AddLiveOrder(pOrder);
		}
		else
		{
			m_FreeList.push_back(pOrder);
		}
	}
}


void CRomOrders::GetMapFileName(unsigned short hdID)
{
	m_MapFileName = CConfig::getInstance().MapFile();
	int index = m_MapFileName.find_last_of('.');
	char hdIdStr[3];
	itoa(hdID, hdIdStr, 10);
	m_MapFileName.insert(index - 1, hdIdStr);
}

void CRomOrders::DestroyOrder(CRomOrder* pOrder)
{
	RemoveFromMaps(pOrder);
	pOrder->SetUsed(false);
	m_FreeList.push_back(pOrder);
	memset(pOrder, 0, sizeof(CRomOrder));
}

void CRomOrders::DestroyOrders(std::vector<CRomOrder*>& orders)
{
	CRomOrder* pOrder = NULL;
	int index = 0;
	while (index < orders.size())
	{
		pOrder = orders[index];
		DestroyOrder(pOrder);
		index++;
	}
}

void CRomOrders::DestroyOrderByRomTag(const char* tag)
{
	CRomOrder* pOrder = NULL;
	tTagOrderMapIterator it = m_RomTagToOrder.find(tag);

	if (it != m_RomTagToOrder.end())
	{
		pOrder = (*it).second;
		DestroyOrder(pOrder);
	}
}

void CRomOrders::DestroyOrderByClientTag(const char* tag)
{
	CRomOrder* pOrder = NULL;
	tTagOrderMapIterator it = m_ClientTagToOrder.find(tag);

	if (it != m_ClientTagToOrder.end())
	{
		pOrder = (*it).second;
		DestroyOrder(pOrder);
	}
}

CRomOrder* CRomOrders::FindOrderFromRomTag(const char* tag)
{
	CRomOrder* rc = NULL;
	
	tTagOrderMapIterator it = m_RomTagToOrder.find(std::string(tag));
	if (it != m_RomTagToOrder.end())
	{
		rc = (*it).second;
	}

	return rc;
}

CRomOrder* CRomOrders::FindOrderFromClientTag(const std::string& tag)
{
	CRomOrder* rc = NULL;
	tTagOrderMapIterator it = m_ClientTagToOrder.find(tag);

	if (it != m_ClientTagToOrder.end())
	{
		rc = (*it).second;
	}

	return rc;
}
/*
bool CRomOrders::LoadFromMap()
{
	bool rc = false;

	if (MapFileExist())
	{
		rc = AllocateMemoryMap(0);
	}

	return rc;
}

void CRomOrders::CleanUp()
{
	if(m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if(m_hFileMap)
		CloseHandle(m_hFileMap);

	if(m_hFileHandle)
		CloseHandle(m_hFileHandle);

	ClearMaps();
}*/

bool CRomOrders::MapFileExist()
{
	std::string file = CConfig::getInstance().MapFile().c_str();
	HANDLE h = CreateFile(	(CConfig::getInstance().MapFile()).c_str(),
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,	
								NULL
							   );
	if (INVALID_HANDLE_VALUE == h)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CRomOrders::AllocateMemoryMap()
{
	ClearMaps();

	unsigned int oldNumOrderSlots = m_nAllocatedOrders;
	m_nAllocatedOrders += m_AllocateStep;
	unsigned int newSize = sizeof(OrdersMemoryStruct) + (m_nAllocatedOrders - 1) * sizeof(CRomOrder);

	m_MemMap.sync();
	m_MemMap.close();

	m_MemMap.map( m_MapFileName.c_str(), 
				  newSize,
				  O_RDWR | O_CREAT,
				  ACE_DEFAULT_FILE_PERMS,
				  PROT_RDWR,
				  ACE_MAP_SHARED);

	m_pMemoryStruct = (OrdersMemoryStruct*)m_MemMap.addr();

	int i = 0;
	CRomOrder* pOrder = NULL;
	
	for (i = 0; i < oldNumOrderSlots; i++)
	{
		pOrder = &((m_pMemoryStruct->Orders)[i]);
		if (pOrder->Used())
		{
			StoreOrder(pOrder);
		}
		else
		{
			m_FreeList.push_back(pOrder);
		}
	}

	unsigned long sizeDelt = sizeof(CRomOrder) * (m_nAllocatedOrders - oldNumOrderSlots);
	memset(&((m_pMemoryStruct->Orders)[oldNumOrderSlots]), 0, sizeDelt);

	for (i = oldNumOrderSlots; i < m_nAllocatedOrders; i++)
	{
		pOrder = &((m_pMemoryStruct->Orders)[i]);
		m_FreeList.push_back(pOrder);
	}
}


CRomOrder* CRomOrders::GetOrderSlot()
{
	if (m_FreeList.empty())
	{
		AllocateMemoryMap();
	}

	CRomOrder* pOrder = m_FreeList.front();
	m_FreeList.pop_front();
	pOrder->SetUsed(true);
	return pOrder;
}

void CRomOrders::StoreOrder(CRomOrder* pOrder)
{
	m_RomTagToOrder.insert(tTagOrderMap::value_type(pOrder->RomTag(), pOrder));

	if (strlen(pOrder->ParentRomTag()) == 0)
	{
		char value[200];
		sprintf(value, "%s%s", pOrder->Owner(), pOrder->ClientTag());
			
		m_ClientTagToOrder.insert(tTagOrderMap::value_type(std::string(value), pOrder));
		
		tOwnerOrderListMapIterator it = m_OwnerToOrderList.find(pOrder->Owner());

		if (it != m_OwnerToOrderList.end())
		{
			((*it).second)->insert(pOrder);
		}
		else
		{
			tOrderList* pList = new tOrderList;
			m_OwnerToOrderList.insert(tOwnerOrderListMap::value_type(pOrder->Owner(), pList));
			pList->insert(pOrder);
		}

		tDestOrderListMapIterator itt = m_DestToOrderList.find(pOrder->Dest());

		if (itt != m_DestToOrderList.end())
		{
			((*itt).second)->insert(pOrder);
		}
		else
		{
			tOrderList* pList = new tOrderList;
			m_DestToOrderList.insert(tDestOrderListMap::value_type(pOrder->Dest(), pList));
			pList->insert(pOrder);
		}
	}
}


void CRomOrders::RemoveFromMaps(CRomOrder* pOrder)
{
	tTagOrderMapIterator it = m_RomTagToOrder.find(pOrder->RomTag());

	if (it != m_RomTagToOrder.end())
	{
		m_RomTagToOrder.erase(it);
	}

	if (strlen(pOrder->ParentRomTag()) == 0)
	{
		char value[200];
		sprintf(value, "%s%s", pOrder->Owner(), pOrder->ClientTag());
		it = m_ClientTagToOrder.find(value);

		if (it != m_ClientTagToOrder.end())
		{
			m_ClientTagToOrder.erase(it);
		}

		tOwnerOrderListMapIterator itt = m_OwnerToOrderList.find(pOrder->Owner());
		if (itt != m_OwnerToOrderList.end())
		{
			tOrderList* pOrders = (*itt).second;
			pOrders->erase(pOrder);
		}

		tDestOrderListMapIterator ittt = m_DestToOrderList.find(pOrder->Dest());
		if (ittt != m_DestToOrderList.end())
		{
			tOrderList* pOrders = (*ittt).second;
			pOrders->erase(pOrder);
		}
	}
}


//void CRomOrders::StoreOrder(CRomOrder* pOrder)
//{
	//pOrder->SetRomTag((NextRomTag(pOrder->Owner())).c_str());

	//Since all the action requests from client or Admin are targeting the parent client tag
    //there is no need to store legs in clienttagmap, ownermap or destmap.
	//if (pOrder->P->Parent() == NULL)
	//if (strlen(pOrder->ParentRomTag()) == 0)
	//{
	//	InsertIntoMaps(pOrder);
	//}
//}




/*std::string CRomOrders::NextRomTag(const char* owner)
{
	char value[200];
	sprintf(value, "%s-%s-%d", owner, m_currentDay.c_str(),  m_RomOrderId++);
	
	DWORD delt = m_SavedRomOrderId - m_RomOrderId;
		
	if (delt < 5)
	{
		m_SavedRomOrderId += ORDER_ID_STEP;
		SaveRomOrderId(m_SavedRomOrderId);
	}

	return std::string(value); 
}

void CRomOrders::LoadCurrentDayStr(SYSTEMTIME& systime)
{
	char date[9];
	wsprintf(date, "%04u%02u%02u", systime.wYear, systime.wMonth, systime.wDay);
	std::string sDate = &(date[1]);
	m_currentDayStr = CConfig::getInstance().sInstance() + sDate;
}

std::string CRomOrders::NextRomTag(const char* owner)
{
	SYSTEMTIME systime;
    GetSystemTime(&systime); //this should be done through a timer to minimize performance impact
	
	if (systime.wDay != m_mDay)
	{
		m_mDay = systime.wDay;
		LoadCurrentDayStr(systime);
	}

	char value[200];
	sprintf(value, "%s-%s-%I64d", owner, m_currentDayStr.c_str(),  m_orderNum++);
	return std::string(value); 
}

__int64 CRomOrders::GetInitialOrderNum()
{
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	__int64 seconds = 3600 * systime.wHour + 60 * systime.wMinute + systime.wSecond;
	__int64 rc = 1000 * (1000 * seconds + systime.wMilliseconds + 16);
	return rc;
}


void CRomOrders::OpenReg()
{
	std::stringstream stm;
	stm << "Software\\Ronin\\ROM" << CConfig::getInstance().Instance();
	std::string key = stm.str();

	if (RegCreateKey( HKEY_LOCAL_MACHINE, 
					  (const char*)key.c_str(), 
					  &m_RegistryHandle) != ERROR_SUCCESS)
	{
		// log error
	}
}

void CRomOrders::CloseReg()
{
	RegCloseKey(m_RegistryHandle);
}

void CRomOrders::SaveRomOrderId(DWORD value)
{
	char buf[20];
	_itoa_s(value, buf, 10);

	RegSetValueEx( m_RegistryHandle, 
				   "Order ID",
				   0,
				   REG_SZ,
				   (const unsigned char*)buf,
				   strlen(buf) + 1);
}

DWORD CRomOrders::GetSavedRomOrderId()
{
	char buf[64];
	DWORD type = REG_SZ;
	DWORD size = sizeof(buf);

	RegQueryValueEx(m_RegistryHandle,
					"Order ID",
					0,
					&type,
					(unsigned char*)buf,
					&size);

	return atoi(buf);
}
*/

void CRomOrders::CancelClientAll(const char* owner, short dest)
{
	tOwnerOrderListMapIterator it = m_OwnerToOrderList.find(owner);

	if (it != m_OwnerToOrderList.end())
	{
		tOrderList* pList = (*it).second;
		tOrderListIterator itt = pList->begin();
		CRomOrder* pOrder = NULL;
		std::string err;

		while (itt != pList->end())
		{
			pOrder = *itt;
			itt++;
			if ((dest != -1 &&
				 pOrder->Dest() != dest) ||
				 //pOrder->Status() == ROMVALUE::STATUS_FILLED)
				 CRomOrder::IsDead(pOrder) == true)
			{
				continue;
			}

			/*
			CRomCancel cancel(*pOrder);
			//cancel.SetAt(ROMFIELDS::CSV_OUTGOING_SEQ, GetNextDestOutgoingSeqNum(pOrder->Dest()));
			unsigned long seq = 0;
			if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
			{
				pOrder->SetCancelSeq(seq);
			}*/
		}
	}
}


void CRomOrders::CancelAll()
{
	
}


void CRomOrders::GetLiveOrders(const std::string& login, std::list<std::string>& clientQ)
{
	tOwnerOrderListMapIterator it = m_OwnerToOrderList.find(login.c_str());
	tOrderList* pList = NULL;
	CRomOrder* pOrder = NULL;

	if (it != m_OwnerToOrderList.end())
	{
		pList = (*it).second;
		tOrderListIterator itt = pList->begin();
		while (itt != pList->end())
		{
			pOrder = *itt;
			//if (pOrder->Status() != ROMVALUE::STATUS_FILLED)
			if (CRomOrder::IsDead(pOrder) == false)
			{
				clientQ.push_back(pOrder->Message());
			}
			itt++;
		}
	}
}

void CRomOrders::Cancels(const char* owner, short dest)
{
	tOwnerOrderListMapIterator it = m_OwnerToOrderList.find(owner);

	if (it != m_OwnerToOrderList.end())
	{
		tOrderList* pList = (*it).second;
		tOrderListIterator itt = pList->begin();
		CRomOrder* pOrder = NULL;
		std::string err;

		while (itt != pList->end())
		{
			pOrder = *itt;
			if (pOrder->Dest() == dest &&
				pOrder->Status() != ROMVALUE::STATUS_FILLED)
			{
			/*	CRomCancel cancel(*pOrder);
				
				unsigned long seq = 0;
				if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
				{
					pOrder->SetCancelSeq(seq);
				}*/
			}
			itt++;
		}
	}
}


void CRomOrders::CancelDestAll(short dest, bool cancelGTDC)
{
    tDestOrderListMapIterator it = m_DestToOrderList.find(dest);
    
    if (it != m_DestToOrderList.end())
    {
        tOrderList* pList = (*it).second;
        tOrderListIterator itt = pList->begin();
        CRomOrder* pOrder = NULL;
		std::string err;

        while (itt != pList->end())
        {
            pOrder = *itt;

			if (CRomOrder::IsDead(pOrder) == false)
			{
				if (false == cancelGTDC)
				{
					if (pOrder->TIF() != ROMVALUE::TIF_GTD &&
						pOrder->TIF() != ROMVALUE::TIF_GTC)
					{
						/*CRomCancel cancel(*pOrder);
						unsigned long seq = 0;
						if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
						{
							pOrder->SetCancelSeq(seq);
						}*/
					}
				}
				else
				{
					/*CRomCancel cancel(*pOrder);
					unsigned long seq = 0;
					if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
					{
						pOrder->SetCancelSeq(seq);
					}*/
				}
			}

            itt++;
        }
    }
}

/*
void CRomOrders::ConnectParent()
{
    tTagOrderMapIterator it = m_RomTagToOrder.begin();
    CRomOrder* pChild = NULL, *pParent = NULL;

    while (it != m_RomTagToOrder.end())
    {
        pChild = it->second;
		if (strlen(pChild->ParentRomTag()) != 0)
        {
            pParent = FindOrderFromRomTag(pChild->ParentRomTag());
            if (pParent)
            {
                pChild->InheritParent(pParent);
                pParent->AddChild(pChild);
				pChild->SetParent(pParent);
            }
        }
        it++;
    }
}*/


void CRomOrders::UpdateClientTag(const char* oldtag, const char* newtag, const char* owner)
{
    if (strcmp(newtag, "") == 0 ||
		strcmp(oldtag, newtag) == 0)
    {
        return;
    }

	char value[100];
	sprintf(value, "%s%s", owner, oldtag);
			
	CRomOrder* pOrder = FindOrderFromClientTag(std::string(value));
    if (pOrder)
    {
        m_ClientTagToOrder.erase(value);
		sprintf(value, "%s%s", owner, newtag);
		m_ClientTagToOrder.insert(tTagOrderMap::value_type(std::string(value), pOrder));
    }
}

void CRomOrders::ClearMaps()
{
	m_RomTagToOrder.clear();
	m_ClientTagToOrder.clear();

	tDestOrderListMapIterator it1 = m_DestToOrderList.begin();
	while (it1 != m_DestToOrderList.end())
	{
		tOrderList* pList = it1->second;
		if (pList)
		{
			pList->clear();
			delete pList;
		}
		it1++;
	}
	m_DestToOrderList.clear();

	tOwnerOrderListMapIterator it2 = m_OwnerToOrderList.begin();
	while (it2 != m_OwnerToOrderList.end())
	{
		tOrderList* pList = it2->second;
		if (pList)
		{
			pList->clear();
			delete pList;
		}
		it2++;
	}
	m_OwnerToOrderList.clear();
}


CRomOrder* CRomOrders::CreateOrder(CRomCsv& csv)
{
	CRomOrder* pOrder = GetOrderSlot();

	pOrder->SingleSet(csv);
	//std::string romTag = NextRomTag(pOrder->Owner());
	//csv.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romTag);
	//pOrder->SetRomTag(romTag.c_str());
	StoreOrder(pOrder);
	
	/*if (csv.HasChildren() == true)
	{
		StoreOrder(pOrder);
		RFIX::CRomMessage<CDummy>::tChild& children = csv.GetChildren();
		RFIX::CRomMessage<CDummy>::tChildIterator it = children.begin();
		RFIX::CRCsv* pM = NULL;
		CRomOrder* pChild = NULL;
	    
		while (it != children.end())
		{
			pM = *it;
			CRomCsv childCsv(pM->Message());
			childCsv.SetAt(ROMFIELDS::CSV_OWNER, csv.Get(ROMFIELDS::CSV_OWNER));
			pChild = CreateOrder(childCsv);
			pChild->InheritParent(pOrder);
			//pOrder->AddChild(pChild);
			it++;
		}

		switch(pOrder->CplxType())
		{
		case ROMVALUE::CPLXTYPE_SPREAD:
			SpreadOrderPostSet(pOrder);
			break;
		case ROMVALUE::CPLXTYPE_CROSS:
			CrossOrderPostSet(pOrder);
			break;
		case ROMVALUE::CPLXTYPE_BASKET:
			BasketOrderPostSet(pOrder);
			break;
		default:
			break;
		}
	}*/

	return pOrder;
}

/*
void CRomOrders::SpreadOrderPostSet(CRomOrder* pOrder)
{
	if (pOrder->HasChildren() == false)
	{
		return;
	}

	CRomOrder::tChildrenIT it = (pOrder->Children())->begin();
	while (it != (pOrder->Children())->end())
	{
		CRomOrder* pChild = *it;
		pChild->SetTrader(pOrder->Trader());
		pChild->SetFirm(pOrder->Firm());
		pChild->SetLocalAccount(pOrder->LocalAccount());
		pChild->SetClearingAccount(pOrder->ClearingAccount());
		pChild->SetExchange(pOrder->Exchange());
		pChild->SetDest(pOrder->Dest());
		pChild->SetExDest(pOrder->ExDest());
		pChild->SetTIF(pOrder->TIF());
		pChild->SetSecType(pOrder->SecType());
		pChild->SetTradeFor(pOrder->TradeFor());
		pChild->SetCMTAAccount(pOrder->CMTAAccount());
		pChild->SetGiveUpAccount(pOrder->GiveUpAccount());
		pChild->SetMMAccount(pOrder->MMAccount());
		pChild->SetMMExchange(pOrder->MMExchange());
		pChild->SetClearingId(pOrder->ClearingId());
		pChild->SetCapacity(pOrder->Capacity());
		pChild->SetExecInst( pOrder->ExecInst() );
		pChild->SetLeave(pOrder->Shares() * pChild->Shares());
		pChild->SetParent(pOrder);
		pChild->SetParentRomTag(pOrder->RomTag());
		it++;
	}
}
*/
void CRomOrders::CrossOrderPostSet(CRomOrder* pOrder)
{
}

void CRomOrders::BasketOrderPostSet(CRomOrder* pOrder)
{
}


void CRomOrders::TestSize()
{
	int FreeListSize = m_FreeList.size();
	int RomTagSize = m_RomTagToOrder.size();
	int ClientTagSize = m_ClientTagToOrder.size();
	int OwnerListSize = m_OwnerToOrderList.size();
    int DestListSize = m_DestToOrderList.size();
}

/*void CRomOrders::SetDestLastOutgoingSeqNum(unsigned short destID, unsigned long value)
{
	if (destID < 300)
	{
		(m_pMemoryStruct->DestSeqNum[destID]).m_LastOutgoing = value;
	}
}

void CRomOrders::SetDestLastIncomingSeqNum(unsigned short destID, unsigned long value)
{
	if (destID < 300)
	{
		(m_pMemoryStruct->DestSeqNum[destID]).m_LastIncoming = value;
	}
}

unsigned long CRomOrders::GetNextDestOutgoingSeqNum(unsigned short destID)
{
	unsigned long rc = 0;

	if (destID < 300)
	{
		rc = ++((m_pMemoryStruct->DestSeqNum[destID]).m_LastOutgoing);
	}

	return rc;
}

unsigned long CRomOrders::DestLastOutgoingSeqNum(unsigned short destID)
{
	unsigned long rc = 0;

	if (destID < 300)
	{
		rc = (m_pMemoryStruct->DestSeqNum[destID]).m_LastOutgoing;
	}

	return rc;
}

void CRomOrders::DecrementLastOutgoingSeqNum(unsigned short destID)
{
	if (destID < 300)
	{
		(m_pMemoryStruct->DestSeqNum[destID]).m_LastOutgoing--;
	}
}

void CRomOrders::IncrementDestLastIncomingSeqNum(unsigned short destID)
{
	if (destID < 300)
	{
		(m_pMemoryStruct->DestSeqNum[destID]).m_LastIncoming++;
	}
}

unsigned long CRomOrders::DestLastIncomingSeqNum(unsigned short destID)
{
	unsigned long rc = 0;

	if (destID < 300)
	{
		rc = (m_pMemoryStruct->DestSeqNum[destID]).m_LastIncoming;
	}

	return rc;
}*/


void CRomOrders::ProcessGapFill(short destID, unsigned long newLastSent)
{
	tDestOrderListMapIterator it = m_DestToOrderList.find(destID);
	if (it != m_DestToOrderList.end())
	{
		CRomOrder* pOrder = NULL;
		tOrderListIterator it1 = (it->second)->begin();
		std::vector<CRomOrder*> orders;
		std::string omExTag, err;
		CRomCsv csv;

		while (it1 != (it->second)->end())
		{
			pOrder = *it1;
			if (pOrder->OrderSeq() > newLastSent &&
				pOrder->Status() == ROMVALUE::STATUS_NEW)
			{
				//Reject order to client;
				pOrder->GetCsv(csv);
				err = "Order was not received by exchange";
				std::string owner = pOrder->Owner();
				CRomReject reject(csv, err);
				pOrder->NextRomExecTag(omExTag);
				reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
				//m_pEnging->SendToClientWithSeq(owner, reject);
				//m_pEnging->DoOats(&reject, pOrder);
				orders.push_back(pOrder);

				//Need to get it out of trader's balance!!!
				//m_pEnging->UpdateTraderBalance(reject, pOrder);

				if (pOrder->CancelSeq() > newLastSent)
				{
					//Generate cancel and send it to exchange
					err = "Order was note received by exchange";
					csv.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
					CRomReject reject(csv, err);
					reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
					//m_pEnging->SendToClientWithSeq(owner, reject);
					//m_pEnging->DoOats(&reject, pOrder);
				}

				if (pOrder->ReplaceSeq() > newLastSent)
				{
					err = "Replace was not received by exchange";
					csv.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_REPLACE);
					CRomReject reject(csv, err);
					short destID = pOrder->Dest();
					reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
					//m_pEnging->SendToClientWithSeq(owner, reject);
					//m_pEnging->DoOats(&reject, pOrder);
				}
			}
			else 
			{
				if (pOrder->CancelSeq() > newLastSent)
				{
					//Generate cancel and send it to exchange
					CRomCancel cancel(*pOrder);
					//short destID = pOrder->Dest();
					pOrder->SetCancelSeq(0);
					//GetNextDestOutgoingSeqNum(destID);

					unsigned long seq = 0;
					//if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
					//{
					//	pOrder->SetCancelSeq(seq);
					//}
				}

				if (pOrder->ReplacePending() &&
					pOrder->ReplaceSeq() > newLastSent)
				{
					pOrder->GetCsv(csv);
					err = "Replace was not received by exchange";
					csv.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_REPLACE);
					csv.SetAt(ROMFIELDS::CSV_NEW_TAG, pOrder->NewClientTag());
					CRomReject reject(csv, err);
					short destID = pOrder->Dest();
					reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
					std::string owner = pOrder->Owner();
					
					//m_pEnging->SendToClientWithSeq(owner, reject);
					//m_pEnging->DoOats(&reject, pOrder);
					pOrder->SetReplaceSeq(0);
				}
			}

			it1++;
		}

		if (orders.size() > 0)
		{
			DestroyOrders(orders);
		}
	}
}

void CRomOrders::Cancels(CRomCsv& filter, bool fromAdmin)
{
	short dest = (filter.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST) == true)? -1 : filter.nGet(ROMFIELDS::CSV_ORDERDEST);
	const char* owner = (filter.IsFieldEmpty(ROMFIELDS::CSV_OWNER) == true)? NULL : filter.Get(ROMFIELDS::CSV_OWNER);
	tOrderList* pOrderList = NULL;
	tOrderList orderList;

	if (owner != NULL)
	{
		tOwnerOrderListMapIterator it = m_OwnerToOrderList.find(owner);
		if (it != m_OwnerToOrderList.end())
		{
			pOrderList = it->second;
		}
	}

	if (NULL == pOrderList &&
		dest != -1)
	{
		tDestOrderListMapIterator it = m_DestToOrderList.find(dest);
		if (it != m_DestToOrderList.end())
		{
			pOrderList = it->second;
		}
	}

	if (NULL == pOrderList)
	{
		return;
	}
	//if (!pOrderList)
	//{
	//	tTagOrderMapIterator it = m_RomTagToOrder.begin();
	//	while (it != m_RomTagToOrder.end())
	//	{
	//		orderList.insert(it->second);
	//		it++;
	//	}
	//	pOrderList = &orderList;
	//}

	tOrderListIterator it = pOrderList->begin();
	CRomOrder* pOrder = NULL;

	std::string err = "";
	while (it != pOrderList->end())
	{
		pOrder = *it;

		if (pOrder->Status() == ROMVALUE::STATUS_FILLED)
		{
			it++;
			continue;
		}

		if (Filter(*pOrder, filter, fromAdmin) == false)
		{
			CRomCancel cancel(*pOrder);
			cancel.SetAt(ROMFIELDS::CSV_OWNERCANCEL, filter.nGet(ROMFIELDS::CSV_OWNERCANCEL));

			unsigned long seq = 0;
			//if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
			//{
			//	pOrder->SetCancelSeq(seq);
			//}
			//m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, err);

			cancel.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
			cancel.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
			pOrder->SetActClientTag(cancel.Get(ROMFIELDS::CSV_NEW_TAG));
			std::string omExTag;
			pOrder->NextRomExecTag(omExTag);
			cancel.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
			//m_pEnging->DoOats(&cancel, pOrder);
		}
		it++;
	}
}

bool CRomOrders::Filter(const CRomOrder& order, CRomCsv& filter, bool fromAdmin)
{
	//if (filter.IsFieldEmpty(ROMFIELDS::CSV_OM_TIME) == false)
	//{
	//	unsigned int starttime = GetStartTime(filter.GetAt(ROMFIELDS::CSV_OM_TIME));
	//	unsigned int endtime = GetEndTime(filter.GetAt(ROMFIELDS::CSV_OM_TIME));
	//}  Don't do time range now because of the time zoon difference between ROM and OrderMaster

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE) == false)
	{
		if (false == fromAdmin)
		{
			if (order.SecType() != filter.cGet(ROMFIELDS::CSV_SECURITY_TYPE))
			{
				return true;
			}
		}
		else
		{
			char secType = filter.cGet(ROMFIELDS::CSV_SECURITY_TYPE);
			
			if ((CRomOrder::IsEquityType(secType) == true &&
				 CRomOrder::IsEquityType(order.SecType()) != true) ||
				(CRomOrder::IsOptionType(secType) == true &&
				 CRomOrder::IsOptionType(order.SecType()) != true) ||
				(CRomOrder::IsFutureType(secType) == true &&
				 CRomOrder::IsFutureType(order.SecType()) != true) ||
				(CRomOrder::IsCurrencyType(secType) == true &&
				 CRomOrder::IsCurrencyType(order.SecType()) != true)  ||
				(secType == ROMVALUE::SECTYPE_COMPLEX &&
				 order.CplxType() == ROMVALUE::CPLXTYPE_SINGLE))
			{
				return true;
			}
		}
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_EXCHANGE) == false &&
		order.Exchange() != filter.nGet(ROMFIELDS::CSV_EXCHANGE))
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST) == false &&
		order.Dest() != filter.nGet(ROMFIELDS::CSV_ORDERDEST))
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_TRADER) == false &&
		strcmp(order.Trader(), filter.Get(ROMFIELDS::CSV_TRADER)) != 0)
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_TRADE_FOR) == false &&
		strcmp(order.TradeFor(), filter.Get(ROMFIELDS::CSV_TRADE_FOR)) != 0)
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_CLEARINGACCOUNT) == false &&
		strcmp(order.ClearingAccount(), filter.Get(ROMFIELDS::CSV_CLEARINGACCOUNT)) != 0)
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_SYMBOL) == false &&
		strcmp(order.Symbol(), filter.Get(ROMFIELDS::CSV_SYMBOL)) != 0)
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_SIDE) == false &&
		order.Side() != filter.nGet(ROMFIELDS::CSV_SIDE))
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_TYPE) == false &&
		order.Type() != filter.nGet(ROMFIELDS::CSV_TYPE))
	{
		return true;
	} 

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false &&
		order.Price() != filter.dGet(ROMFIELDS::CSV_PRICE))
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_ORIGINAL_SHARES) == false &&
		order.OriginalShares() != filter.nGet(ROMFIELDS::CSV_ORIGINAL_SHARES))
	{
		return true;
	}

	if (filter.IsFieldEmpty(ROMFIELDS::CSV_TIF) == false &&
		order.TIF() != filter.nGet(ROMFIELDS::CSV_TIF))
	{
		return true;
	}

	if (CRomOrder::IsOptionType(order.SecType()))
	{
		if (filter.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING) == false &&
			strcmp(order.Underlying(), filter.Get(ROMFIELDS::CSV_UNDERLYING)) != 0)
		{
			return true;
		}

		if (filter.IsFieldEmpty(ROMFIELDS::CSV_STRIKEPRICE) == false &&
			order.StrikePrice() != filter.dGet(ROMFIELDS::CSV_STRIKEPRICE))
		{
			return true;
		}

		if (filter.IsFieldEmpty(ROMFIELDS::CSV_CALLPUT) == false &&
			order.CallOrPut() != filter.cGet(ROMFIELDS::CSV_CALLPUT))
		{
			return true;
		}
	}

	if (CRomOrder::IsEquityType(order.SecType()) == false)
	{
		if (filter.IsFieldEmpty(ROMFIELDS::CSV_EXPYEARMONTH) == false &&
			strcmp(order.ExpYearMonth(), filter.Get(ROMFIELDS::CSV_EXPYEARMONTH)) != 0)
		{
			return true;
		}
	}

	return false;
}

