#include "OrderManager.h"
#include "ClientProfileManager.h"
#include "ClientsManager.h"
#include <string>
//#include "Dbo.h"
#include "RomCancel.h"

//#include "generic_db.h"

//#include "RomLog.h"


COrderManager::COrderManager():
m_ClientTagToHIDMap(true),
m_OrderNum(1, true)
{
}

void COrderManager::Init()
{
  InitHandlers();
	InitOrderNum();
}

COrderManager::~COrderManager()
{
}

//void COrderManager::UpdateAllDestRoutes(CRomMap<unsigned short, unsigned short>& newMap)
//{
//}

void COrderManager::CancelDestOrders(const std::string& destID)
{
  CRomCancel* pCsv = new CRomCancel(destID);
  AddCancels(pCsv);
}

void COrderManager::InitHandlers()
{
	bool loadOrders = false;

	if (CConfig::getInstance().IsFirstRun() == false &&
		CConfig::getInstance().LoadOrdersFromDB() == false) 
	{
		loadOrders = true;
	}

  m_Handlers.resize(CConfig::getInstance().NumOrderHandler());
	for (int i = 0; i < m_Handlers.size(); i++)
	{
		m_Handlers[i] = new COrderHandler(i, loadOrders);
		m_Handlers[i]->Start();
	}

	if (!loadOrders)
	{
		//Load Orders from DB
		//LoadOrdersFromDB();
    LoadOrdersFromDB_New();
	}
}


unsigned long long COrderManager::GetNextRomTag(const std::string& owner, std::string& romTag)
{
  unsigned long long rc = m_OrderNum.Get();
  char value[200];
#ifdef _WIN32
  sprintf(value, "%s-%s-%I64d", owner.c_str(), CConfig::getInstance().CurrentDayStr().c_str(),  rc);
#else
  sprintf(value, "%s-%s-%lu", owner.c_str(), CConfig::getInstance().CurrentDayStr().c_str(),  rc);
#endif
	romTag = value; 
  return rc;
}

unsigned long long COrderManager::GetNextLegRomTag(const std::string& parentRomTag, std::string& romTag)
{
  int offset = parentRomTag.find_last_of('-');
  std::string ordNumStr = parentRomTag.substr(offset + 1);
  std::string baseTag = parentRomTag.substr(0, offset);
  unsigned long long orderNum = 0;
  char value[200];

#ifdef _WIN32
		orderNum = _atoi64(ordNumStr.c_str());
    orderNum = m_OrderNum.Get(CConfig::getInstance().NumOrderHandler());
    sprintf(value, "%s-%I64d", baseTag.c_str(), orderNum);

#else
    orderNum = atoll(ordNumStr.c_str());
    orderNum = m_OrderNum.Get(CConfig::getInstance().NumOrderHandler());
    sprintf(value, "%s-%lu", baseTag.c_str(), orderNum);
#endif

    romTag = value;
    return orderNum;
}



bool COrderManager::AddApprovedOrder(CCsv* pCsv)
{
  bool rc = false;

	unsigned long long ordNum;
  std::string romtag;
  pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
  if (GetOrderNumFromRomTag(romtag, ordNum))
  {
    m_Handlers[ordNum % m_Handlers.size()]->AddMessage(pCsv);
    //m_Handlers[ordNum % m_Handlers.size()]->ProcessSingleOrder(pCsv);
    rc = true;
  }
  return rc;
}

bool COrderManager::AddOrderRequest(CCsv* pCsv)
{
	bool rc = false;
  std::string owner;
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
	std::string romtag;

	unsigned long long ordnum = GetNextRomTag(owner, romtag);
  pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);

  CCsv::tChildrenIterator it = pCsv->GetChildren().begin();
  CCsv* pcsv = NULL;
  std::string childRomTag;

  while (it != pCsv->GetChildren().end())
  {
    GetNextLegRomTag(romtag, childRomTag);
    pcsv = *it;
    pcsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, childRomTag);
    pcsv->SetAt(ROMFIELDS::CSV_OWNER, owner);
    it++;
  }

  std::string tag;
  pCsv->GetAt(ROMFIELDS::CSV_TAG, tag);
  std::string realTag = owner + tag;
  unsigned short HID = ordnum % m_Handlers.size();
  m_ClientTagToHIDMap.Add(realTag, HID);

	//send the order to clientprofile manager for approve
	//CClientProfileManager::getInstance().AddOrderRequest(pCsv);

  if (CClientProfileManager::getInstance().ProcessOrderRequest(pCsv))
  {
    AddApprovedOrder(pCsv);
  }
  else
  {
    pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
    ReturnNotApprovedOrder(pCsv);
  }

	return rc;
}

bool COrderManager::AddReplaceOrCancelOrZap(CCsv* pCsv)
{
	bool rc = false;
  std::string tag, romtag;

	if (pCsv->GetAt(ROMFIELDS::CSV_TAG, tag) == true)
	{
		unsigned short hID;
    std::string owner;
    pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
    std::string realTag = owner + tag;
    if (GetHIDThroughClientTag(realTag, hID))
    {
		  m_Handlers[hID]->AddMessage(pCsv);
    }
    else
    {
      pCsv->SetAt(ROMFIELDS::CSV_COMMAND, "S");
      pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Order not found");
      std::string dummy;
      CClientsManager::getInstance().Send(owner, *pCsv, dummy);
      rc = true;
    }
	}
  else if (pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag) == true)
  {
    unsigned long long ordNum;
    GetOrderNumFromRomTag(romtag, ordNum);
	  return m_Handlers[ordNum % m_Handlers.size()]->AddMessage(pCsv);
  }

	return rc;
}

bool COrderManager::AddCancels(CCsv* pCsv)
{
  CCsv* pCancels = NULL;
	for (int i = 0; i < m_Handlers.size(); i++)
	{
    pCancels = new CCsv(*pCsv);
		m_Handlers[i]->AddMessage(pCancels);
	}

	return true;
}


void COrderManager::AddClientRequest(CCsv* pCsv)
{
	bool rc = true;
  char cmd = ROMVALUE::COMMAND_ORDER;
  pCsv->GetAt(ROMFIELDS::CSV_COMMAND, cmd);

	switch(cmd)
	{
	case ROMVALUE::COMMAND_ORDER:
		rc = AddOrderRequest(pCsv);
		break;
	case ROMVALUE::COMMAND_CANCEL:
	case ROMVALUE::COMMAND_REPLACE:
		rc = AddReplaceOrCancelOrZap(pCsv);
		break;
	case ROMVALUE::COMMAND_CANCELS:
		rc = AddCancels(pCsv);
		break;
	default:
    break;
	}

  if (rc)
  {
    delete pCsv;
  }
}

//bool COrderManager::AddHostUpdate(CCsv* pCsv)
bool COrderManager::AddHostUpdate(CCsv* pCsv)
{
  //CCsv* pCsv = new CCsv(pMsg, len);
	unsigned long long ordNum;
  std::string romtag;
  pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
  GetOrderNumFromRomTag(romtag, ordNum);
	return m_Handlers[ordNum % m_Handlers.size()]->AddMessage(pCsv);
  //return m_Handlers[ordNum % m_Handlers.size()]->ProcessHostUpdate(pCsv);
}

unsigned long long COrderManager::GetNextOrderNum()
{
	return m_OrderNum.Get();
}

bool COrderManager::GetOrderNumFromRomTag(const std::string& tag, unsigned long long& orderNum)
{
	bool rc = false;
	int index = tag.find_last_of('-');

	if (index != std::string::npos &&
		index + 1 < tag.size())
	{
		std::string ordNumStr = tag.substr(index + 1);
#ifdef _WIN32
		orderNum = _atoi64(ordNumStr.c_str());
#else
    orderNum = atoll(ordNumStr.c_str());
#endif
    rc = true;
	}

	return rc;
}

bool COrderManager::GetHIDThroughClientTag(const std::string& tag, unsigned short& hID)
{
  return m_ClientTagToHIDMap.Get(tag, hID);
}


void COrderManager::InitOrderNum()
{
	if (CConfig::getInstance().IsFirstRun() == false)
	{
    time_t ticks = time(NULL);
    struct tm *tmStruct = gmtime(&ticks);
    unsigned long long seconds = 3600 * tmStruct->tm_hour + 60 * tmStruct->tm_min + tmStruct->tm_sec + 1;
	  m_OrderNum.Reset(1000000 * seconds);
	}
}

bool COrderManager::LoadOrdersFromDB_New()
{
  bool rc = true;
  const char* pUser = (CConfig::getInstance().dbLogin()).c_str();
  const char* pPwd = (CConfig::getInstance().dbPassword()).c_str();
  const char* pServer = (CConfig::getInstance().dbServer()).c_str();
  queue_t results = create_queue();
  queue_t row = NULL;

  int rt = gendb_get_results( pUser, 
                              strlen(pUser),
                              pPwd, 
                              strlen(pPwd),
                              pServer, 
                              strlen(pServer),
                              results, 
                              "rom.dbo.OM_selOrdersNew");

  if (!rt)
  {
    gendb_v* pVal = NULL;
    element_t rowE, colE;
    		
    while (is_empty(results) == 0)
    {
      int cnt = 0;
      rowE = pop_front(results);
      row = (queue_t)(rowE->d);
      CRomOrder order;
      memset(&order, 0, sizeof(CRomOrder));
      bool skip = false;

      while (is_empty(row) == 0)
      {
        colE = pop_front(row);
        pVal = (gendb_v*)(colE->d);

        double un_double;
        int un_int;
        char un_char;
        char un_array[256];

       				
        switch(cnt)
        {
        case 0:
          order.SetAvgPrice((pVal->g_un).un_double);
          break;
        case 1:
          order.SetCallOrPut((pVal->g_un).un_array[0]);
          break;
        case 2:
          order.SetCapacity((pVal->g_un).un_array[0]);
          break;
        case 3:
          order.SetClearingAccount((pVal->g_un).un_array);
          break;
        case 4:
          order.SetClearingId((pVal->g_un).un_array);
          break;
        case 5:
          order.SetTimeStamp((pVal->g_un).un_array);
          break;
        case 6:
          order.SetCMTAAccount((pVal->g_un).un_array);
          break;
        case 7:
          order.SetCum((pVal->g_un).un_int);
          break;
        case 8:
          order.SetDBSymbol((pVal->g_un).un_array);
          break;
        case 9:
          {
            char destStr[4];
            sprintf(destStr, "%d", (pVal->g_un).un_int);
        	  order.SetExchange(destStr);
          }
          break;
        case 10:
          {
            char destStr[4];
            sprintf(destStr, "%d", (pVal->g_un).un_int);
        	  order.SetDest(destStr);
          }
          break;
        case 11:
          order.SetExDest((pVal->g_un).un_array);
          break;
        case 12:
          order.SetExpYearMonth((pVal->g_un).un_array);
          break;
        case 13:
          order.SetFirm((pVal->g_un).un_array);
          break;
        case 14:
          order.SetGiveUpAccount((pVal->g_un).un_array);
          break;
        case 15:
          order.SetInstructions((pVal->g_un).un_array);
          break;
        case 16:
          order.SetLeave((pVal->g_un).un_int);
          order.SetShares(order.Cum() + order.Leave());
          break;
        case 17:
          order.SetLocalAccount((pVal->g_un).un_array);
          break;
        case 18:
          {
            char maxFloor[10];
            sprintf(maxFloor, "%d", (pVal->g_un).un_int);
				    order.SetMaxFloor(maxFloor);
          }
          break;
        case 19:
          order.SetMinQty((pVal->g_un).un_int);
          break;
        case 20:
          order.SetMMAccount((pVal->g_un).un_array);
          break;
        case 21:
          order.SetMMExchange((pVal->g_un).un_int);
          break;
        case 22:
          order.SetMultiplier((pVal->g_un).un_int);
          break;
        case 23:
          if (strcmp("JOEL-170101206-8", (pVal->g_un).un_array) == 0)
          {
            bool hit = true;
          }
          if (strcmp((pVal->g_un).un_array, "") == 0)
				  {
            skip = true;
				  }
          else
          {
				    short romInstance = CUtility::GetRomIDFromRomTag((pVal->g_un).un_array);
                
            if (romInstance != CConfig::getInstance().Instance())
            {
                skip = true; // Do not pick up the orders belonging to other ROM instances!!
            }
            else
            {
              order.SetRomTag((pVal->g_un).un_array);
            }
          }
          break;
        case 24:
           order.SetOpenClose((pVal->g_un).un_int);
          break;
        case 25:
          order.SetExpTime((pVal->g_un).un_array);
          break;
        case 26:
          order.SetRomExchangeTag((pVal->g_un).un_array);
          break;
        case 27:
          order.SetType((pVal->g_un).un_int);
          break;
        case 28:
          order.SetOriginalShares((pVal->g_un).un_int);
          break;
        case 29:
          order.SetOwner((pVal->g_un).un_array);
          break;
        case 30:
          order.SetPrice((pVal->g_un).un_double);
          break;
        case 31:
          order.SetSecType((pVal->g_un).un_array[0]);
          break;
        case 32:
          order.SetSide((pVal->g_un).un_int);
          break;
        case 33:
          order.SetStatus((pVal->g_un).un_int);
          break;
        case 34:
          order.SetStopPrice((pVal->g_un).un_array);
          break;
        case 35:
          order.SetStrikePrice((pVal->g_un).un_double);
          break;
        case 36:
          order.SetSymbol((pVal->g_un).un_array);
          break;
        case 37:
          order.SetClientTag((pVal->g_un).un_array);
          break;
        case 38:
          order.SetTIF((pVal->g_un).un_int);
          break;
        case 39:
          order.SetTradeFor((pVal->g_un).un_array);
          break;
        case 40:
          order.SetTrader((pVal->g_un).un_array);
          break;
        case 41:
          order.SetCplxType((pVal->g_un).un_int);
          break;
        case 42:
          if (strcmp((pVal->g_un).un_array, "") != 0)
				  {
					  order.SetExecInst((pVal->g_un).un_array[0]);
				  }
				  break;
        case 43:
          if (strcmp((pVal->g_un).un_array, "") != 0)
				  {
					  order.SetNextExecIdFromExecTag((pVal->g_un).un_array);
				  }
				  else
				  {
					  order.SetNextExecId(0);
				  }
          break;
        case 44:
          order.SetParentRomTag((pVal->g_un).un_array);
          break;
        case 45:
          order.SetSecID((pVal->g_un).un_array);
          break;
        case 46:
          order.SetCplxType((pVal->g_un).un_int);
          break;
        case 47:
          if (pVal->type_in_union != DB_NULL)
          {
            char pegoffset[10];
					  sprintf(pegoffset, "%f", (pVal->g_un).un_double);
					  order.SetPegOffset(pegoffset);
          }
          break;
        case 48:
          if (pVal->type_in_union != DB_NULL)
				  {
					  order.SetSecDef((pVal->g_un).un_array);
				  }
          break;
        case 49:
          if (pVal->type_in_union != DB_NULL)
				  {
					  if (CRomOrder::IsOptionType(order.SecType()))
					  {
						  if (strlen((pVal->g_un).un_array) == 8)
						  {
							  order.SetMaturityDay(&((pVal->g_un).un_array[6]));
						  }
					  }
					  else
					  {
						  if (strlen((pVal->g_un).un_array) >= 6)
						  {
							  std::string buf = (pVal->g_un).un_array;
							  order.SetExpYearMonth(buf.substr(0, 6).c_str());
						  }
					  }
				  }
          break;
        case 50:
          if (pVal->type_in_union != DB_NULL)
				  {
					  order.SetAlgoType((pVal->g_un).un_int);
				  }
          break;
        default:
          break;
        }
 
        cnt++;
      }

      if (true == skip)
      {
        break;
      }

      if (strcmp(order.ParentRomTag(), "") == 0 &&
					order.CplxType() != ROMVALUE::CPLXTYPE_SPREAD)
			{
				CClientProfileManager::getInstance().AddLiveOrder(order);
			}

			AddOrder(&order);

      destroy_queue(row);
    }

    destroy_queue(results);
  }
  else
  {
    rc = false;
  }

  return rc;
}
/*
void COrderManager::LoadOrdersFromDB()
{
	DboConnection * pDB = new DboConnection( (CConfig::getInstance().dbServer()).c_str(), (CConfig::getInstance().dbLogin()).c_str(), (CConfig::getInstance().dbPassword()).c_str(), "OM" );
	if( !pDB )
	{
		//RomLog::LogMessage("DB Error ", "DboConnection can not be created");
		return;
	}

	DboCommand cmd( pDB, "rom.dbo.OM_selOrdersNew" );

	//RomLog::LogMessage("DB", "Calling OM_selOrdersNew...");

	cmd.exec();

	if( cmd.status() != DBO_SUCCESS )
	{
		//RomLog::LogMessage("DB Error ", "OM_selOrdersNew failed");
		delete pDB;
		return;
	}
	else
	{
		//RomLog::LogMessage("DB", "OM_selOrdersNew returned successfully");
	}


 //O.avgPrice,
 //1.callPut,
 //2.capacity,
 //3.clearingAccount,
 //4.clearingID,
 //5.convert(varchar,OH.clientTime,108),
 //6.CMTAID,
 //7.cumQty,
 //8.dbSymbol,
 //9.DestID,
 //10.DestRouteID,
 //11.exDest,
 //12.expDate,
 //13.firm,
 //14.GiveUp,
 //15.instructions,
 //16.leavesQty,
 //17.localAcct,
 //18.maxFloor,
 //19.minQty,
 //20.mmAcct,
 //21.mmExchange,
 //22.multiplier,
 //23.omTag,
 //24.openClose,
 //25.orderExpiresDate,
 //26.orderTag,
 //27.orderType,
 //28.originalShares,
 //29.owner,
 //3O.price,
 //31.secType,
 //32.side,
 //33.status,
 //34.stopPrice,
 //35.strikePrice,
 //36.symbol,
 //37.tag,
 //38.tif,
 //39.tradeFor,
 //40.trader,
 //41.underlying
 //42.ExecInst
 //43.OMExecTag
 //44.ParentStaffordTag
 //45.SecID
 //46.CplxOrderType
 //47.PegOffSet
 //48.SecDef
 //49.expireDate
 5//0.algotype

	// Retrieve open orders and store them to the map file
	DboResults * pRes = NULL;
	char szBuff[48];
	int nTemp = 0;
	double dTemp = 0;

	long resultsCount = 0;

	CRomOrder* pOrder = NULL;

	for( int k = 0; k < cmd.resultsCount(); k++ )
	{
		pRes = cmd.results(k);
		resultsCount += pRes->rowCount();
	}

	for( int i = 0; i < cmd.resultsCount(); i++ )
	{
		pRes = cmd.results(i);
		for( int j = 0; j < pRes->rowCount(); j++ )
		{
			try
			{
				pRes->rowSet( j );

				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 23, szBuff, sizeof(szBuff));
				if (strcmp(szBuff, "") == 0)
				{
					continue;
				}

				short romInstance = CUtility::GetRomIDFromRomTag(szBuff);
                
                if (romInstance != CConfig::getInstance().Instance())
                {
                    continue;  // Do not pick up the orders belonging to other ROM instances!!
                }

				CRomOrder order;
                CRomOrder* pOrder = &order; //GetOrderSlot();
	            pOrder->SetRomTag(szBuff);

				dTemp = 0;
				pRes->colValue(0, &dTemp);
                pOrder->SetAvgPrice(dTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 1, szBuff, sizeof(szBuff));
				pOrder->SetCallOrPut(szBuff[0]);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 2, szBuff, sizeof(szBuff));
				pOrder->SetCapacity(szBuff[0]);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 3, szBuff, sizeof(szBuff));
				pOrder->SetClearingAccount(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 4, szBuff, sizeof(szBuff));
				pOrder->SetClearingId(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 5, szBuff, sizeof(szBuff));
				pOrder->SetTimeStamp(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 6, szBuff, sizeof(szBuff));
				pOrder->SetCMTAAccount(szBuff);
				
				nTemp = 0;
				pRes->colValue( 7, &nTemp);
                pOrder->SetCum(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 8, szBuff, sizeof(szBuff));
				pOrder->SetDBSymbol(szBuff);
				
				nTemp = 0;
				pRes->colValue( 9, &nTemp );
        char destStr[4];
        sprintf(destStr, "%d", nTemp);
        //itoa(nTemp, destStr, 10);
				pOrder->SetExchange(destStr);
				
				nTemp = 0;
				pRes->colValue( 10, &nTemp );
        sprintf(destStr, "%d", nTemp);
        //itoa(nTemp, destStr, 10);
				pOrder->SetDest(destStr);
				
				//the 12 should be ignored after we release Oats for OSI
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 12, szBuff, sizeof(szBuff));
				pOrder->SetExpYearMonth(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 11, szBuff, sizeof(szBuff));
				pOrder->SetExDest(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 13, szBuff, sizeof(szBuff));
				pOrder->SetFirm(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 14, szBuff, sizeof(szBuff));
				pOrder->SetGiveUpAccount(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 15, szBuff, sizeof(szBuff));
				pOrder->SetInstructions(szBuff);
				
				nTemp = 0;
				pRes->colValue( 16, &nTemp );
                pOrder->SetLeave(nTemp);
                pOrder->SetShares(pOrder->Cum() + pOrder->Leave());
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 17, szBuff, sizeof(szBuff));
				pOrder->SetLocalAccount(szBuff);
				
				nTemp = 0;
				pRes->colValue( 18, &nTemp );

				char maxFloor[10];
        sprintf(maxFloor, "%d", nTemp);
				//_itoa(nTemp, maxFloor, 10);
        pOrder->SetMaxFloor(maxFloor);
				
				nTemp = 0;
				pRes->colValue( 19, &nTemp );
                pOrder->SetMinQty(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 20, szBuff, sizeof(szBuff));
				pOrder->SetMMAccount(szBuff);
				
				nTemp = 0;
				pRes->colValue( 21, &nTemp );
				pOrder->SetMMExchange(nTemp);
				
				nTemp = 0;
				pRes->colValue( 22, &nTemp );
				pOrder->SetMultiplier(nTemp);
				
				nTemp = 0;
				pRes->colValue( 24, &nTemp );
				pOrder->SetOpenClose(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 25, szBuff, sizeof(szBuff));
				pOrder->SetExpTime(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 26, szBuff, sizeof(szBuff));
				pOrder->SetRomExchangeTag(szBuff);
				
				nTemp = 0;
				pRes->colValue( 27, &nTemp );
				pOrder->SetType(nTemp);
				
				nTemp = 0;
				pRes->colValue( 28, &nTemp );
                pOrder->SetOriginalShares(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 29, szBuff, sizeof(szBuff));
				pOrder->SetOwner(szBuff);
				
				dTemp = 0;
				pRes->colValue( 30, &dTemp );
                pOrder->SetPrice(dTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 31, szBuff, sizeof(szBuff));
				pOrder->SetSecType(szBuff[0]);
				
				nTemp = 0;
				pRes->colValue( 32, &nTemp );
				pOrder->SetSide(nTemp);
				
				nTemp = 0;
				pRes->colValue( 33, &nTemp );
				pOrder->SetStatus(nTemp);
				
				dTemp = 0;
				pRes->colValue( 34, &dTemp );
				char stopPrice[10];
				sprintf(stopPrice, "%f", dTemp);
                pOrder->SetStopPrice(stopPrice);
				
				dTemp = 0;
				pRes->colValue( 35, &dTemp );
                pOrder->SetStrikePrice(dTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 36, szBuff, sizeof(szBuff));
				pOrder->SetSymbol(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 37, szBuff, sizeof(szBuff));
				pOrder->SetClientTag(szBuff);
				
				nTemp = 0;
				pRes->colValue( 38, &nTemp );
				pOrder->SetTIF(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 39, szBuff, sizeof(szBuff));
				pOrder->SetTradeFor(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 40, szBuff, sizeof(szBuff));
				pOrder->SetTrader(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 41, szBuff, sizeof(szBuff));
				pOrder->SetUnderlying(szBuff);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 42, szBuff, sizeof(szBuff));
				if (strcmp(szBuff, "") == 0)
				{
					strcpy(szBuff, "-");
				}
				pOrder->SetExecInst(szBuff[0]);
			
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 43, szBuff, sizeof(szBuff));
				if (strcmp(szBuff, "") != 0)
				{
					//pOrder->SetRomExecTag(szBuff);
					pOrder->SetNextExecIdFromExecTag(szBuff);
				}
				else
				{
					pOrder->SetNextExecId(0);
				}

				nTemp = 0;
				pRes->colValue( 46, &nTemp );
				pOrder->SetCplxType(nTemp);
				
				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 45, szBuff, sizeof(szBuff));
				pOrder->SetSecID(szBuff);
				
				dTemp = 0;
				if (pRes->colValue(47, &dTemp) == DBO_SUCCESS)
				{
					char pegoffset[10];
					sprintf(pegoffset, "%f", dTemp);
					pOrder->SetPegOffset(pegoffset);
				}

				memset(szBuff, 0, sizeof(szBuff));
				if (pRes->colValue( 48, szBuff, sizeof(szBuff)) == DBO_SUCCESS)
				{
					pOrder->SetSecDef(szBuff);
				}

				memset(szBuff, 0, sizeof(szBuff));
				if (pRes->colValue( 49, szBuff, sizeof(szBuff)) == DBO_SUCCESS)
				{
					if (CRomOrder::IsOptionType(pOrder->SecType()))
					{
						if (strlen(szBuff) == 8)
						{
							pOrder->SetMaturityDay(&(szBuff[6]));
						}
					}
					else
					{
						if (strlen(szBuff) >= 6)
						{
							std::string buf = szBuff;
							pOrder->SetExpYearMonth(buf.substr(0, 6).c_str());
						}
					}
				}

				nTemp = 0;
				if (pRes->colValue( 50, &nTemp )  == DBO_SUCCESS)
				{
					pOrder->SetAlgoType(nTemp);
				}

				memset(szBuff, 0, sizeof(szBuff));
				pRes->colValue( 44, szBuff, sizeof(szBuff));
				pOrder->SetParentRomTag(szBuff);
				
				if (strcmp(szBuff, "") == 0 &&
					pOrder->CplxType() != ROMVALUE::CPLXTYPE_SPREAD)
				{
					CClientProfileManager::getInstance().AddLiveOrder(*pOrder);
				}

				AddOrder(pOrder);
			}
			catch(...)
			{
				continue;
			}
		}
	}

	//ConnectParent();

	delete pDB;
	//return true;
}
*/
void COrderManager::AddOrder(CRomOrder* pOrder)
{
  unsigned long long ordNum;
  GetOrderNumFromRomTag(pOrder->RomTag(), ordNum);
  ordNum = ordNum % m_Handlers.size();
  std::string realTag = std::string(pOrder->Owner()) + std::string(pOrder->ClientTag());
  m_ClientTagToHIDMap.Add(realTag, ordNum);
	m_Handlers[ordNum]->AddOrder(pOrder);
}

void COrderManager::AddToClientTagToHIDMap(CRomOrder* pOrder)
{
  unsigned long long ordNum;
  GetOrderNumFromRomTag(pOrder->RomTag(), ordNum);
  ordNum = ordNum % m_Handlers.size();
  std::string realTag = std::string(pOrder->Owner()) + std::string(pOrder->ClientTag());
  m_ClientTagToHIDMap.Add(realTag, ordNum);
}

void COrderManager::RemoveClientTag(const std::string& tag)
{
  unsigned short dummy;
  m_ClientTagToHIDMap.Remove(tag, dummy);
}

void COrderManager::UpdateClientTag(const std::string& oldtag, const std::string& newtag)
{
  m_ClientTagToHIDMap.ReplaceKey(oldtag, newtag);
}

bool COrderManager::ReturnNotApprovedOrder(CCsv* pCsv)
{
  unsigned short dummy;
  std::string owner, tag, err;
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
  pCsv->GetAt(ROMFIELDS::CSV_TAG, tag);
  std::string realtag = owner + tag;
  m_ClientTagToHIDMap.Remove(realtag, dummy);
  return CClientsManager::getInstance().Send(owner, *pCsv, err);
}
