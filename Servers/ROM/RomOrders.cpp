#ifdef _WIN32
#include "stdafx.h"
#endif

#include "RomOrders.h"
#include "RomCancel.h"
#include "RomReject.h"
#include "ClientProfileManager.h"
//#include "Dbo.h"
#include "DestsManager.h"
#include "OrderManager.h"


CRomOrders::CRomOrders(unsigned short hdID, bool loadOrder):
m_hdID(hdID),
m_RomTagToOrder(false),
m_ClientTagToOrder(false),
m_OwnerToOrderSet(false),
m_DestToOrderSet(false)
/*m_RomTagToOrder(false),
m_ClientTagToOrder(false),
m_OwnerToOrderSet(false),
m_DestToOrderSet(false)*/
{
#ifdef _WIN32
  m_hFileHandle = NULL;
	m_hFileMap = NULL;
	m_pFileView = NULL;
#else
  m_hFileHandle = -1;
#endif
 
  sprintf(m_MapFileName, "%s_%d", (CConfig::getInstance().MapFile()).c_str(), m_hdID);
  m_curSlotsNum = 0;

  if (false == loadOrder)
  {
    DeleteMapFile();
  }
  
  AllocateMemoryMap(false);
}

void CRomOrders::DeleteMapFile()
{
#ifdef _WIN32
  DeleteFile(m_MapFileName);
#else
  remove(m_MapFileName);
#endif
}

//bool CRomOrders::LoadFromMap()
//{
//  return AllocateMemoryMap(false);
//}




CRomOrders::~CRomOrders(void)
{
	ClearMaps();
	//CloseReg();
}



void CRomOrders::DestroyOrder(CRomOrder* pOrder)
{
	RemoveFromMaps(pOrder);
	pOrder->SetUsed(false);
	m_FreeList.push_back(pOrder);
  std::string tag = pOrder->Owner();
  tag.append(pOrder->ClientTag(), strlen(pOrder->ClientTag()));
	COrderManager::getInstance().RemoveClientTag(tag);
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
	
  if (m_RomTagToOrder.Get(tag, pOrder))
	{
		DestroyOrder(pOrder);
	}
}

void CRomOrders::DestroyOrderByClientTag(const char* tag)
{
	CRomOrder* pOrder = NULL;
	
	if (m_ClientTagToOrder.Get(tag, pOrder))
	{
		DestroyOrder(pOrder);
	}
}

CRomOrder* CRomOrders::FindOrderFromRomTag(const char* tag)
{
	CRomOrder* rc = NULL;
	m_RomTagToOrder.Get(tag, rc);
	return rc;
}

CRomOrder* CRomOrders::FindOrderFromClientTag(const std::string& tag)
{
	CRomOrder* rc = NULL;
  m_ClientTagToOrder.Get(tag, rc);
	return rc;
}
CRomOrder* CRomOrders::GetOrderSlot()
{
	if (m_FreeList.empty())
	{
	  AllocateMemoryMap(true);
	}

	CRomOrder* pOrder = m_FreeList.front();
	m_FreeList.pop_front();
	pOrder->SetUsed(true);
	return pOrder;
}

void CRomOrders::StoreOrder(CRomOrder* pOrder)
{
	m_RomTagToOrder.Add(pOrder->RomTag(), pOrder);

	if (strlen(pOrder->ParentRomTag()) == 0)
	{
		char value[200];
		sprintf(value, "%s%s", pOrder->Owner(), pOrder->ClientTag());
			
		m_ClientTagToOrder.Add(value, pOrder);
		
    tOrderSet* pOrderSet = NULL;
		
		if (m_OwnerToOrderSet.Get(pOrder->Owner(), pOrderSet))
		{
			pOrderSet->insert(pOrder);
		}
		else
		{
			pOrderSet = new tOrderSet;
			m_OwnerToOrderSet.Add(pOrder->Owner(), pOrderSet);
			pOrderSet->insert(pOrder);
		}

    if (m_DestToOrderSet.Get(pOrder->Dest(), pOrderSet))
		{
			pOrderSet->insert(pOrder);
		}
		else
		{
			pOrderSet = new tOrderSet;
			m_DestToOrderSet.Add(pOrder->Dest(), pOrderSet);
			pOrderSet->insert(pOrder);
		}
	}
}


void CRomOrders::RemoveFromMaps(CRomOrder* pOrder)
{
  CRomOrder* pDummy = NULL;

  m_RomTagToOrder.Remove(pOrder->RomTag(), pDummy);

	if (strlen(pOrder->ParentRomTag()) == 0)
	{
    tOrderSet* pOrders = NULL;
		char value[200];
		sprintf(value, "%s%s", pOrder->Owner(), pOrder->ClientTag());
		m_ClientTagToOrder.Remove(value, pDummy);
		
    if (m_OwnerToOrderSet.Get(pOrder->Owner(), pOrders))
    {
       tOrderSetIt it = pOrders->find(pOrder);
       if (it != pOrders->end())
       {
         pOrders->erase(it);
       }
    }

    if (m_DestToOrderSet.Get(pOrder->Dest(), pOrders))
    {
       tOrderSetIt it = pOrders->find(pOrder);
       if (it != pOrders->end())
       {
         pOrders->erase(it);
       }
    }
	}
}

void CRomOrders::CancelClientAll(const char* owner, std::string dest)
{
	tOrderSet* pOrders = NULL;
	if (m_OwnerToOrderSet.Get(owner, pOrders))
	{
		tOrderSetIt it = pOrders->begin();
		CRomOrder* pOrder = NULL;
		std::string err;

		while (it != pOrders->end())
		{
			pOrder = *it;
			it++;
			if ((dest != "-1" &&
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
  tOrderSet* pOrders = NULL;
	CRomOrder* pOrder = NULL;

	if (m_OwnerToOrderSet.Get(login, pOrders))
	{
		tOrderSetIt it = pOrders->begin();
		while (it != pOrders->end())
		{
			pOrder = *it;
			if (CRomOrder::IsDead(pOrder) == false)
			{
				clientQ.push_back(pOrder->Message());
			}
			it++;
		}
	}
}

/*
void CRomOrders::Cancels(const char* owner, std::string dest)
{
	tOrderSet* pOrders = NULL;

	if (m_OwnerToOrderSet.Get(owner, pOrders))
	{
		tOrderSetIt it = pOrders->begin();
		CRomOrder* pOrder = NULL;
		std::string err;

		while (it != pOrders->end())
		{
			pOrder = *it;
      it++;
			if (dest.compare(pOrder->Dest()) == true &&
				  pOrder->Status() != ROMVALUE::STATUS_FILLED)
			{
				CRomCancel cancel(*pOrder);
				
				unsigned long seq = 0;
				if (m_pEnging->SendToDestWithSeq(pOrder->Dest(), &cancel, seq, err))
				{
					pOrder->SetCancelSeq(seq);
				}
			}
		}
	}
}*/


void CRomOrders::CancelDestAll(std::string dest, bool cancelGTDC)
{
  tOrderSet* pOrders = NULL;
  if (m_DestToOrderSet.Get(dest, pOrders))
  {
    tOrderSetIt it = pOrders->begin();
    CRomOrder* pOrder = NULL;
	  std::string err;

    while (it != pOrders->end())
    {
      pOrder = *it;

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

      it++;
    }
  }
}

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
    CRomOrder* pDummy = NULL;
    m_ClientTagToOrder.Remove(value, pDummy);
	  sprintf(value, "%s%s", owner, newtag);
	  m_ClientTagToOrder.Add(value, pOrder);
  }
}

void CRomOrders::ClearMaps()
{
	m_RomTagToOrder.RemoveAll();
	m_ClientTagToOrder.RemoveAll();

  tOrderSet* pOrders = NULL;

	while ((pOrders = (tOrderSet*)m_DestToOrderSet.RemoveNext()) != NULL)
	{
		delete pOrders;
	}
	
	while ((pOrders = (tOrderSet*)m_OwnerToOrderSet.RemoveNext()) != NULL)
	{
		delete pOrders;
	}
}


CRomOrder* CRomOrders::CreateOrder(CCsv& csv)
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
		ROMFIELDS::CRomMessage<CDummy>::tChild& children = csv.GetChildren();
		ROMFIELDS::CRomMessage<CDummy>::tChildIterator it = children.begin();
		ROMFIELDS::CRCsv* pM = NULL;
		CRomOrder* pChild = NULL;
	    
		while (it != children.end())
		{
			pM = *it;
			CCsv childCsv(pM->Message());
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


void CRomOrders::CrossOrderPostSet(CRomOrder* pOrder)
{
}

void CRomOrders::BasketOrderPostSet(CRomOrder* pOrder)
{
}


void CRomOrders::TestSize()
{
	int FreeListSize = m_FreeList.size();
	int RomTagSize = m_RomTagToOrder.Size();
	int ClientTagSize = m_ClientTagToOrder.Size();
	int OwnerListSize = m_OwnerToOrderSet.Size();
  int DestListSize = m_DestToOrderSet.Size();
}


void CRomOrders::Cancels(CCsv& filter, std::list<COatsMsg*> oatsMsgs)
{
  std::string dest = "-1"; //(filter.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST) == true)? "-1" : filter.GetAt(ROMFIELDS::CSV_ORDERDEST);
  filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
  std::string owner; //(filter.IsFieldEmpty(ROMFIELDS::CSV_OWNER) == true)? NULL : filter.Get(ROMFIELDS::CSV_OWNER);
  tOrderSet* pOrders = NULL;
	tOrderSet Orders;

	if (filter.GetAt(ROMFIELDS::CSV_OWNER, owner) == true)
	{
		m_OwnerToOrderSet.Get(owner, pOrders);
	}

	if (NULL == pOrders &&
		  dest != "-1")
	{
		m_DestToOrderSet.Get(dest, pOrders);
	}

	if (NULL == pOrders)
	{
		return;
	}
	
  tOrderSetIt it = pOrders->begin();
	CRomOrder* pOrder = NULL;

	std::string err = "";
  
	while (it != pOrders->end())
	{
		pOrder = *it;

		if (pOrder->Status() == ROMVALUE::STATUS_FILLED)
		{
			it++;
			continue;
		}

		if (Filter(*pOrder, filter) == false)
		{
			CRomCancel cancel(*pOrder);
      if (strcmp(pOrder->Owner(), owner.c_str()) == 0)
      {
        short ownercancel = 0;
        filter.GetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
			  cancel.SetAt(ROMFIELDS::CSV_OWNERCANCEL, ownercancel);
      }

      CDestsManager::getInstance().Send(pOrder->Dest(), cancel, err);

			cancel.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
			cancel.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
      std::string newtag;
      cancel.GetAt(ROMFIELDS::CSV_NEW_TAG, newtag);
			pOrder->SetActClientTag(newtag.c_str());
			std::string omExTag;
			pOrder->NextRomExecTag(omExTag);
			cancel.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
      COatsMsg* pMsg = new COatsMsg(&cancel, pOrder, true);
      oatsMsgs.push_back(pMsg);
			//m_pEnging->DoOats(&cancel, pOrder);
		}
		it++;
	}
}

bool CRomOrders::Filter(const CRomOrder& order, CCsv& filter)
{
	//if (filter.IsFieldEmpty(ROMFIELDS::CSV_OM_TIME) == false)
	//{
	//	unsigned int starttime = GetStartTime(filter.GetAt(ROMFIELDS::CSV_OM_TIME));
	//	unsigned int endtime = GetEndTime(filter.GetAt(ROMFIELDS::CSV_OM_TIME));
	//}  Don't do time range now because of the time zoon difference between ROM and OrderMaster
  char secType;

	if (filter.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType) == true )//filter.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE) == false)
	{
		/*if (false == fromAdmin)
		{
			if (order.SecType() != filter.cGet(ROMFIELDS::CSV_SECURITY_TYPE))
			{
				return true;
			}
		}
		else
		{*/
		
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
//		}
	}

  std::string exchange;
	if (filter.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_EXCHANGE) == false &&
		  strcmp(order.Exchange(), exchange.c_str()) != 0)
	{
		return true;
	}

  std::string dest;
	if (filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_ORDERDEST) == false &&
		  strcmp(order.Dest(), dest.c_str()) != 0)
	{
		return true;
	}
  
  std::string trader;
	if (filter.GetAt(ROMFIELDS::CSV_TRADER, trader) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TRADER) == false &&
		  strcmp(order.Trader(), trader.c_str()) != 0)
	{
		return true;
	}

  std::string tradefor;
	if (filter.GetAt(ROMFIELDS::CSV_TRADE_FOR, tradefor) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TRADE_FOR) == false &&
		  strcmp(order.TradeFor(), tradefor.c_str()) != 0)
	{
		return true;
	}

  std::string clearingacct;
	if (filter.GetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, clearingacct) == true && // filter.IsFieldEmpty(ROMFIELDS::CSV_CLEARINGACCOUNT) == false &&
		  strcmp(order.ClearingAccount(), clearingacct.c_str()) != 0)
	{
		return true;
	}

  std::string symbol;
	if (filter.GetAt(ROMFIELDS::CSV_SYMBOL, symbol) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_SYMBOL) == false &&
		  strcmp(order.Symbol(), symbol.c_str()) != 0)
	{
		return true;
	}

  short side = 0;
	if (filter.GetAt(ROMFIELDS::CSV_SIDE, side) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_SIDE) == false &&
		  order.Side() != side)
	{
		return true;
	}

  short type = 0;
  if (filter.GetAt(ROMFIELDS::CSV_TYPE, type) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TYPE) == false &&
		  order.Type() != type)
	{
		return true;
	} 

  double price = 0;
	if (filter.GetAt(ROMFIELDS::CSV_PRICE, price) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false &&
		  order.Price() != price)
	{
		return true;
	}

  long origshare = 0;
	if (filter.GetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, origshare) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_ORIGINAL_SHARES) == false &&
		  order.OriginalShares() != origshare)
	{
		return true;
	}

  short tif;
	if (filter.GetAt(ROMFIELDS::CSV_TIF, tif) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_TIF) == false &&
		  order.TIF() != tif)
	{
		return true;
	}

	if (CRomOrder::IsOptionType(order.SecType()))
	{
    std::string underlying;
    if (filter.GetAt(ROMFIELDS::CSV_UNDERLYING, underlying) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING) == false &&
			  strcmp(order.Underlying(), underlying.c_str()) != 0)
		{
			return true;
		}

    double strike = 0;
		if (filter.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_STRIKEPRICE) == false &&
			  order.StrikePrice() != strike)
		{
			return true;
		}

    char callput;
    if (filter.GetAt(ROMFIELDS::CSV_CALLPUT, callput) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_CALLPUT) == false &&
			order.CallOrPut() != callput)
		{
			return true;
		}
	}

	if (CRomOrder::IsEquityType(order.SecType()) == false)
	{
    std::string expmonth;
		if (filter.GetAt(ROMFIELDS::CSV_EXPYEARMONTH, expmonth) == true && //filter.IsFieldEmpty(ROMFIELDS::CSV_EXPYEARMONTH) == false &&
			strcmp(order.ExpYearMonth(), expmonth.c_str()) != 0) //filter.Get(ROMFIELDS::CSV_EXPYEARMONTH)) != 0)
		{
			return true;
		}
	}

	return false;
}

void CRomOrders::AddOrder(CRomOrder* pOrder)
{
	CRomOrder* pOrd = GetOrderSlot();
  pOrder->SetUsed(true);
	memcpy(pOrd, pOrder, sizeof(CRomOrder));
	StoreOrder(pOrd);
}


bool CRomOrders::AllocateMemoryMap(bool realloc)
{
#ifdef _WIN32
  bool rc = AllocateMemoryMap_WINDOWS(realloc);
#else
  bool rc = AllocateMemoryMap_UNIX(realloc);
  //bool rc = AllocateMemoryMap_UNIX_New(realloc);
#endif
  return rc;
}

#ifdef _WIN32
bool CRomOrders::AllocateMemoryMap_WINDOWS(bool realloc)
{
  CleanUp();

  unsigned long step = CConfig::getInstance().OrdersAllocStep();
  bool rc = true;
 
	m_hFileHandle = CreateFile(m_MapFileName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								CREATE_NEW,
								FILE_ATTRIBUTE_NORMAL,	
								NULL
							   );

	bool NewFile = m_hFileHandle == INVALID_HANDLE_VALUE ? false : true;
	if(NewFile != true)
	{
		m_hFileHandle = CreateFile( m_MapFileName,
									              GENERIC_READ | GENERIC_WRITE,
									              FILE_SHARE_READ | FILE_SHARE_WRITE,
									              NULL,
									              OPEN_ALWAYS,
									              FILE_ATTRIBUTE_NORMAL,
									              NULL
								                );
		
		int fSize = GetFileSize(m_hFileHandle, NULL);
    m_curSlotsNum = (true == realloc)? fSize / sizeof(ORDER) + step : fSize / sizeof(ORDER);
	}
	
	char memName[125];
	sprintf(memName, "ROM%d_%d_MEM", CConfig::getInstance().Instance(), m_hdID);
	m_hFileMap = CreateFileMapping(	(HANDLE)m_hFileHandle,
									NULL,
									PAGE_READWRITE,
									0,
									(m_curSlotsNum - 1) * sizeof(ORDER) + sizeof(OrdersMemoryStruct),
									memName);

	if (m_hFileMap)
	{
		if (m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0))
		{
			m_pMemoryStruct = (OrdersMemoryStruct *)m_pFileView;
      if (NewFile)
      {
        CRomOrder	*pOrder = NULL;
				for (int i = 0; i < m_curSlotsNum; i++)
				{
					pOrder = &((m_pMemoryStruct->orders[i]).order);
					memset(pOrder, 0, sizeof(CRomOrder));
					pOrder->SetUsed(false);
					m_pMemoryStruct->orders[i].check = i;
					m_FreeList.push_back(pOrder);
				}
      }
      else
      {
        rc = ReloadFromNewMap(realloc);
      }
		}	
	}
	else
	{
		rc = false;
  }

  return rc;
}
#else
bool CRomOrders::AllocateMemoryMap_UNIX(bool realloc)
{
  CleanUp();
  bool rc = false;

  m_hFileHandle = open(m_MapFileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  
  if (m_hFileHandle >= 0)
  {
    unsigned long step = CConfig::getInstance().OrdersAllocStep();
    unsigned long bytesToAdd = step * sizeof(ORDER);
    char* buffer = new char[bytesToAdd];
    memset(buffer, 0, bytesToAdd);
    long offset = 0;
    offset = lseek(m_hFileHandle, 0, SEEK_END);
    m_curSlotsNum = offset / sizeof(ORDER);

    if (realloc)
    {
      m_curSlotsNum += step;
      write(m_hFileHandle, buffer, bytesToAdd); //increase the file size!!
      //m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_SHARED, m_hFileHandle, 0);
      m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_PRIVATE, m_hFileHandle, 0);
      rc = ReloadFromNewMap(realloc);
    }
    else
    {
      if (0 == offset)
      {
        write(m_hFileHandle, buffer, bytesToAdd);
        m_curSlotsNum = step;
        //m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_SHARED, m_hFileHandle, 0);
        m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_PRIVATE, m_hFileHandle, 0);
        CRomOrder	*pOrder = NULL;
				for (int i = 0; i < m_curSlotsNum; i++)
				{
					pOrder = &((m_pMemoryStruct->orders[i]).order);
					pOrder->SetUsed(false);
					m_pMemoryStruct->orders[i].check = i;
					m_FreeList.push_back(pOrder);
				}
      }
      else
      {
        //m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_SHARED, m_hFileHandle, 0);
        m_pMemoryStruct = (OrdersMemoryStruct*)mmap(0, m_curSlotsNum * sizeof(ORDER), PROT_READ | PROT_WRITE, MAP_PRIVATE, m_hFileHandle, 0);
        rc = ReloadFromNewMap(realloc);
      }
    }

    delete[] buffer;
  }
  
  return rc;
}

bool CRomOrders::AllocateMemoryMap_UNIX_New(bool realloc)
{
  CleanUp();
  bool rc = false;
  unsigned long step = CConfig::getInstance().OrdersAllocStep();
  CRomOrder* pGOrder = (CRomOrder*)calloc(sizeof(CRomOrder) * step, 1);
  CRomOrder* pOrder = NULL;

  for (int i = 0; i < step; i++)
	{
    pOrder = &(pGOrder[i]);
		pOrder->SetUsed(false);
		m_FreeList.push_back(pOrder);
	}
  return rc;
}

#endif 

void CRomOrders::CleanUp()
{
  //m_curSlotsNum = CConfig::getInstance().OrdersAllocStep();

#ifdef _WIN32
  if(m_hFileHandle)
  {
		CloseHandle(m_hFileHandle);
  }
  if(m_pFileView)
  {
	  UnmapViewOfFile(m_pFileView);
  }
	if(m_hFileMap)
  {
    CloseHandle(m_hFileMap);
  }
#else
  if (m_hFileHandle != -1)
  {
    munmap((caddr_t)m_pMemoryStruct, m_curSlotsNum * sizeof(ORDER));
    close(m_hFileHandle);
  }
 
#endif
	ClearMaps();
}

bool CRomOrders::ReloadFromNewMap(bool realloc)
{
  unsigned long step = (true == realloc)? CConfig::getInstance().OrdersAllocStep() : 0;
  CRomOrder	*pOrder = NULL;
  //existing orders
  for (int i = 0; i < m_curSlotsNum - step; i++)
  {
    if (m_pMemoryStruct->orders[i].check != i)
		{
			//RomLog::LogMessage("AllocateMemoryMap", "Map file corrupted");
			CleanUp();
			//unsigned long errCode = 0;
			std::string corrupFile = CConfig::getInstance().MapFile() + CUtility::GetLocalTimehhmmss();
      if (CUtility::renamefile(CConfig::getInstance().MapFile(), corrupFile) == false)
			{
				//errCode = GetLastError();
			}
			//m_pCfg->SetGapFillEnabled(false);
			return false;
		}

		pOrder = &(m_pMemoryStruct->orders[i].order);
		if (!(pOrder->Used()))
		{
			m_FreeList.push_back(pOrder);
		}
		else
		{
			StoreOrder(pOrder);
			if (false == realloc) //This is a ROM start
			{
        CClientProfileManager::getInstance().AddLiveOrder(*pOrder);
        COrderManager::getInstance().AddToClientTagToHIDMap(pOrder);
			}
 		}
  }

  //new slots
  for (int k = m_curSlotsNum - step; k < m_curSlotsNum; k++)
  {
    m_pMemoryStruct->orders[k].check  = k;
		pOrder = &(m_pMemoryStruct->orders[k].order);
		memset(pOrder, 0, sizeof(CRomOrder));
		pOrder->SetUsed(false);
		m_FreeList.push_back(pOrder);
  }

	char value[100];
	sprintf(value, "Allocated Orders, The newAllocatedSize=%ld", m_curSlotsNum);
	
	//RomLog::LogMessage("AllocateMemoryMap", value);

  return true;
}