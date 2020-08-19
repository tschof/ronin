#include "OrderHandler.h"
#include "romvaluedefs.h"
#include "RomOrder.h"
#include "Config.h"
#include "ClientsManager.h"
#include "DestsManager.h"
#include "RomReject.h"
#include "ClientProfileManager.h"
#include "OrderManager.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

COrderHandler::COrderHandler(unsigned short id, bool loadOrders)
{
	m_pOrders = new CRomOrders(id, loadOrders);
}

COrderHandler::~COrderHandler()
{
	if (m_pOrders)
	{
		delete m_pOrders;
	}
}




bool COrderHandler::ProcessMessage(tReqList& reqlist)
{
	bool rc = true;   //rc is true only when csvMsg needs to be deleted by caller
  CCsv* pCsv = NULL;
  tReqListIt it = reqlist.begin();

  while (it != reqlist.end())
  {
    pCsv = *it;
    it++;

    int passthrough = 0;
    pCsv->GetAt(ROMFIELDS::CSV_PASSTHROUGH, passthrough);
    if (1 == passthrough)
	  {
		  PassThroughToExchange(pCsv);
      delete pCsv;
		  return rc;
	  }

    char cmd = ROMVALUE::COMMAND_ORDER;
    pCsv->GetAt(ROMFIELDS::CSV_COMMAND, cmd);
	
    switch (cmd)
    {
    case ROMVALUE::COMMAND_ORDER:
      {
        //start of debug: Handler gets the order
#ifndef _WIN32
        struct timeval tv;
        gettimeofday(&tv, NULL);
        char buff[10000];
        memset(buff, 0, sizeof(buff));
        std::string tag, owner;
        pCsv->GetAt(ROMFIELDS::CSV_TAG, tag);
        pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
        sprintf(buff, "OrderHandler gets the order: clientag=%s<%ld>\n", tag.c_str(), tv.tv_sec * 1000000 + tv.tv_usec);
        
        struct message mm;
        mm.name = (char*)calloc(owner.length() + 1, 1);
        strcpy(mm.name, owner.c_str());
        mm.name_len = owner.length();
        mm.data = buff;
        mm.len =strlen(buff);
        dart_rolling_log_coming(&mm, mm.name, mm.name_len);
#endif
        //end of debug

        int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
        pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype); 
		    if (ROMVALUE::CPLXTYPE_SPREAD == cplxtype &&
			      pCsv->HasChildren() == false)
		    {
			    cplxtype = ROMVALUE::CPLXTYPE_PRODUCT_SPREAD;
			    pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);
		    }

		    switch (cplxtype)
		    {
		    case ROMVALUE::CPLXTYPE_SINGLE:
			    rc = ProcessSingleOrder(pCsv);
			    break;
		    case ROMVALUE::CPLXTYPE_SPREAD:
			    rc = ProcessSpreadOrder(pCsv);
			    break;
		    case ROMVALUE::CPLXTYPE_PRODUCT_SPREAD:
			    rc = ProcessSingleOrder(pCsv);
			    break;
		    default:
			    break;
		    }
      }
      
      break;

    case ROMVALUE::COMMAND_CANCEL:
      //pCsv->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 1);
		  rc = ProcessClientCancel(pCsv);
      
		  break;
	  
    case ROMVALUE::COMMAND_CANCELS:
		  //pCsv->SetAt(ROMFIELDS::CSV_OWNERCANCEL, 1);
		  rc = ProcessClientCancels(pCsv);
		  break;
	  
    case ROMVALUE::COMMAND_REPLACE:
      
		  rc = ProcessClientReplace(pCsv);
      break;
	  
    case ROMVALUE::COMMAND_STATUS:
		  rc = ProcessHostUpdate(pCsv);
	    break;

    default:
      rc = true;
    }

    if (true == rc)
    {
      delete pCsv;
    }
  }

	return rc;
}

bool COrderHandler::ProcessHostUpdate(CCsv* pCsv)
{
  bool rc = true;
  std::string romtag, symbol, lastshare, lastprice;
  pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
  pCsv->GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
  pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshare);
  pCsv->GetAt(ROMFIELDS::CSV_LAST_PRICE, lastprice);

	CRomOrder* pOrder = m_pOrders->FindOrderFromRomTag(romtag.c_str());
  
	if (!pOrder)
	{
		int status = 0;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
		if (ROMVALUE::STATUS_FILLED == status ||
			ROMVALUE::STATUS_PARTIALLYFILLED == status)
		{
			//send email
			char sbj[256];
			sprintf(sbj, "Order Not Found in %s(RomTag=%s Symbol=%s LastShares=%s LastPrice=%s) ", CConfig::getInstance().Login().c_str(), romtag.c_str(), symbol.c_str(), lastshare.c_str(), lastprice.c_str());
      std::string msg, subject = sbj;
      pCsv->GetMsg(msg);
      CUtility::SendBizEMail(subject, msg);
      //RomLog::LogMessage(subject, msg.Message());
		}
		else
		{
			std::string subject = "Order Not Found ";
			//RomLog::LogMessage(subject, msg.Message());
		}

		return rc;
	}

  CTraderData* pData = CClientProfileManager::getInstance().GetTraderData(pOrder->Trader());
  int cplxtype = 0;
  pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

	if (cplxtype == ROMVALUE::CPLXTYPE_CBOELEG)
	{
		pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, ROMVALUE::CPLXTYPE_LEG);
	}

  if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_SINGLE)
  {
    rc = ProcessHostSingleUpdate(pCsv, pOrder, pData);
  }
	else if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_LEG && //leg of user defined spread
			     strlen(pOrder->ParentRomTag()) != 0)
	{
		rc = ProcessHostSingleUpdate(pCsv, pOrder, pData);
	}
	else if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD &&
			     cplxtype == ROMVALUE::CPLXTYPE_LEG)
	{
		rc = ProcessHostLegUpdate(pCsv, pOrder, pData);
	}
  else
  {
    rc = ProcessHostContainerUpdate(pCsv, pOrder, pData);
  }

	return rc;
}

bool COrderHandler::ProcessHostLegUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData)
{
  bool rc = false;
  std::string err;
  int nStatus = 0;
  pCsv->GetAt(ROMFIELDS::CSV_STATUS, nStatus);
	std::string omExTag;
	pOrder->NextRomExecTag(omExTag);
	pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
	pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, pOrder->Exchange());
	pCsv->SetAt(ROMFIELDS::CSV_TAG, pOrder->ClientTag());
	pCsv->SetAt(ROMFIELDS::CSV_PARENT_TAG, pOrder->ParentClientTag());
	pCsv->SetAt(ROMFIELDS::CSV_TRADER, pOrder->Trader());
	pCsv->SetAt(ROMFIELDS::CSV_TRADE_FOR, pOrder->TradeFor());
	pCsv->SetAt(ROMFIELDS::CSV_CAPACITY, pOrder->Capacity());
			
	if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
	{
		pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, pOrder->Underlying());
	}

	if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
	{
		pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, pOrder->SecType());
	}

	//m_pClients->SendWithSeq(pOrder->Owner(), msg);
  CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);

	int side = pOrder->Side();
  int csvSide = 0;
  pCsv->GetAt(ROMFIELDS::CSV_SIDE, csvSide);
	pOrder->SetSide(csvSide);

	//DoOats(&msg, pOrder, false);
		
	pOrder->SetSide(side);

	if ((ROMVALUE::STATUS_FILLED == nStatus ||
		 ROMVALUE::STATUS_PARTIALLYFILLED == nStatus) &&
		 pData->RoninMember() == true)
	{
		char value[200];
		sprintf(value, 
				"%s %s %s %s", 
				pOrder->TradeFor(),
				pOrder->ClearingAccount(),
				pOrder->Firm(),
				pOrder->LocalAccount());
		pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);
		//m_pClients->Send("ROMTPOS", msg.Message());
    CClientsManager::getInstance().Send("ROMTPOS", *pCsv, err);
	}

	DoDropCopy(*pCsv, pOrder, pData);
	
	COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false);
	DoOats(pOatsMsg);

  return rc;
}

bool COrderHandler::ProcessHostSingleUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData)
{
  bool rc = false;

  short newstatus = 0;
  pCsv->GetAt(ROMFIELDS::CSV_STATUS, newstatus);
	short oldstatus = pOrder->Status();
	DShares dShares;
  std::string err;

	if (newstatus == ROMVALUE::STATUS_REPLACED)
  {
    if (strlen(pOrder->ParentRomTag()) == 0)
    {
	    m_pOrders->UpdateClientTag(pOrder->ClientTag(), pOrder->NewClientTag(), pOrder->Owner());
    }
  }

	// Update Trader Data
  if (strlen(pOrder->ParentRomTag()) == 0) //cplx order not included yet!!
  {
	  UpdateTraderBalance(*pCsv, pOrder);//, pData);
  }

  std::string lastshare;
  pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshare);
	if (lastshare.find('.') != std::string::npos)
	{
		//Fractional lastShares
		dShares.Set(*pCsv);
	}

	pOrder->Update(*pCsv); // Update order regardless what the response is

	std::string omExTag;
	pOrder->NextRomExecTag(omExTag);
	pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

	if (ROMVALUE::STATUS_PENDINGCANCEL != newstatus &&
		  ROMVALUE::STATUS_PENDINGNEW != newstatus &&
		  ROMVALUE::STATUS_REPLACEPENDING != newstatus)
	{
		if (dShares.active() == true)
		{
			std::string lastShares;
      pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
			std::string leave;
      pCsv->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);
			std::string cum;
      pCsv->GetAt(ROMFIELDS::CSV_CUM_SHARES, cum);

			pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, dShares.Last());
			pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, dShares.Leave());
			pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, dShares.Cum());

			//m_pClients->SendWithSeq(pOrder->Owner(), msg);

			pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
			pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);
			pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, cum);
		}
		//else
		//{
		//	m_pClients->SendWithSeq(pOrder->Owner(), msg);
		//}
    CClientsManager::getInstance().Send(pOrder->Owner(),*pCsv, err);
	}

	DoDropCopy(*pCsv, pOrder, pData);
	//DoOats(&msg, pOrder, false);
  std::list<COatsMsg*> oatsMsgs;
	COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false);
	oatsMsgs.push_back(pOatsMsg);
	rc = false;

	if (ROMVALUE::STATUS_FILLED == newstatus &&
		  pOrder->CancelPending())
	{
		//generate cancel reject
		std::string text = "Order was filled(too later to cancel)";
	
		pOrder->NextRomExecTag(omExTag);

    std::string omextag;
    pCsv->GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omextag);
    std::string sStatus;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, sStatus);
    std::string sText;
    pCsv->GetAt(ROMFIELDS::CSV_TEXT, sText);

		pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
		MakeCancelReject(*pCsv, *pOrder, text);
		//m_pClients->SendWithSeq(pOrder->Owner(), msg);
    CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
		DoDropCopy(*pCsv, pOrder, pData);
		pOatsMsg = new COatsMsg(pCsv, pOrder, true);
    oatsMsgs.push_back(pOatsMsg);

    pCsv->SetAt(ROMFIELDS::CSV_STATUS, sStatus);
    pCsv->SetAt(ROMFIELDS::CSV_TEXT, sText);
    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omextag);
	}

	if ((ROMVALUE::STATUS_FILLED == newstatus ||
		   ROMVALUE::STATUS_PARTIALLYFILLED == newstatus) &&
		  pData->RoninMember() == true)
	{
		char value[200];
		sprintf(value, 
				"%s %s %s %s", 
				pOrder->TradeFor(),
				pOrder->ClearingAccount(),
				pOrder->Firm(),
				pOrder->LocalAccount());

		std::string acct;
    pCsv->GetAt(ROMFIELDS::CSV_ACCOUNT, acct);
		pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);
		CClientsManager::getInstance().Send("ROMTPOS", *pCsv, err);
		pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, acct);
	}

  if (oatsMsgs.size() > 0)
	{
		DoOats(oatsMsgs);
	}

  return rc;
}

bool COrderHandler::ProcessHostContainerUpdate(CCsv* pCsv, CRomOrder* pOrder, CTraderData* pData)
{
  bool rc = false;
  std::string err;
  short newstatus = 0;
  pCsv->GetAt(ROMFIELDS::CSV_STATUS, newstatus);
	

  if (newstatus == ROMVALUE::STATUS_REPLACED)
  {
     m_pOrders->UpdateClientTag(pOrder->ClientTag(), pOrder->NewClientTag(), pOrder->Owner());
  }

	if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
	{
		UpdateTraderBalance(*pCsv, pOrder);//, pData);
	}

	pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, pOrder->CplxType());
	
	if (ROMVALUE::STATUS_FILLED != newstatus &&
		  ROMVALUE::STATUS_PARTIALLYFILLED != newstatus)
	{
		DoDropCopy(*pCsv, pOrder, pData);
	}

	CClientsManager::getInstance().Send(pOrder->Owner(), *pCsv, err);
	COatsMsg* pMsg = new COatsMsg(pCsv, pOrder, false);
	DoOats(pMsg);

  return rc;
}

void COrderHandler::CleanUp()
{
}


bool COrderHandler::PassThroughToExchange(CCsv* pCsv)
{
	return true;
}

void COrderHandler::DoOats(COats::tMsgList& msgs)
{
  COats::getInstance().AddMessages(msgs);
}

void COrderHandler::DoOats(COatsMsg* msg)
{
  COats::getInstance().AddMessage(msg);
}

/*
bool COrderHandler::ValidateCapacity(const CRomOrder* pOrder, std::string& err)
{
  char capacity = pOrder->Capacity();
  char secType;
  CDestsManager::getInstance().GetSecTypeFromRouteId(pOrder->Dest(), secType);
	//char secType = m_pExchanges->GetSecurityType(pOrder->Dest());
	bool rc = true;

	if (ROMVALUE::SECTYPE_OPTION == secType &&
		CConfig::getInstance().IsSpecialOptionDest(pOrder->Dest()) == false)
	{
		if ('B' != capacity &&
			'S' != capacity)
		{
			err = "Option Order is allowed only for Capacity 'S'(MarketMaker) and Capacity 'B'(BrokerDealerFirm)";
			rc = false;
		}
	}
	else
	{
		if ('A' != capacity)
		{
			err = "NON Option Order is allowed only for Capacity 'A'(Agency)";
			rc = false;
		}
	}

	return rc;
}
*/

bool COrderHandler::ProcessSingleOrder(CCsv* pCsv)
{
	bool rc = true;
	CRomOrder* pOrder = NULL;
	std::string err = "";
  COats::tMsgList oatsMsgs;
  std::string owner;
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);

  if (!ValidateOrder(*pCsv, owner, err))
	{
		MakeOrderReject(*pCsv, err);
    CClientsManager::getInstance().Send(owner, *pCsv, err);
  }
	else
	{
		std::string routeID, destID;
    pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
    CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
    pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

		pOrder = m_pOrders->CreateOrder(*pCsv);

		if (CRomOrder::IsCurrencyType(pOrder->SecType()) == true)
		{
			std::string symbol = pOrder->Symbol();
			pOrder->SetCurrency((symbol.substr(0, 3)).c_str());
		}
		else
		{
			pOrder->SetCurrency("USD");
		}

		pOrder->SetCurrencyRate(1);
		pOrder->SetDest(routeID.c_str());

		std::string omExTag;
		pOrder->NextRomExecTag(omExTag);
		pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
		
		COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false);
		rc = false;
		oatsMsgs.push_back(pOatsMsg);

		/*std::string err = "";
		if (ValidateCapacity(pOrder, err) == false)
		{
			pOrder->NextRomExecTag(omExTag);
			CRomReject reject(*pCsv, err);
      reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
			CClientsManager::getInstance().Send(owner, *pCsv, err);

			COatsMsg* pOatsMsg = new COatsMsg(&reject, pOrder, true);
			oatsMsgs.push_back(pOatsMsg);
			DoOats(oatsMsgs);
			m_pOrders->DestroyOrder(pOrder);
			return rc;
		}*/
	

		unsigned long seq = 0;

    if (CDestsManager::getInstance().Send(routeID, *pCsv, err) == false)
		{
			pOrder->NextRomExecTag(omExTag);
			CRomReject reject(*pCsv, err);
      reject.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
      CClientsManager::getInstance().Send(owner, reject, err);
			COatsMsg* pOatsMsg = new COatsMsg(&reject, pOrder, true);
			oatsMsgs.push_back(pOatsMsg);
			m_pOrders->DestroyOrder(pOrder);

      /*double valueChange = 0;
			if (CRomOrder::IsLimitType(pOrder->Type()))
			{
				valueChange = -1 * pOrder->Shares() * pOrder->Price() * pOrder->Multiplier() * pOrder->CurrencyRate();
			}

      tBalanceUpdate update;
      memset(&update, 0, sizeof(tBalanceUpdate));
      update.dollarChange = valueChange;
      update.orderChange = -1;
      update.shareChange = -1 * pOrder->Shares();
      update.side = pOrder->Side();
      CUtility::StrCpy(update.trader, sizeof(update.trader), pOrder->Trader());
      CClientProfileManager::getInstance().AddBalanceUpdate(update);*/
		}
		else
		{
			pOrder->SetOrderSeq(seq);
      CClientProfileManager::getInstance().AddLiveOrder(*pOrder);
		}
	}

	if (oatsMsgs.size() > 0)
	{
		DoOats(oatsMsgs);
	}

	return rc;
}

bool COrderHandler::ProcessSpreadOrder(CCsv* pCsv)
{
	bool rc = true;
  CCsv& msg = *pCsv;
  std::string login, error;
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, login);
  
  if (!ValidateOrder(*pCsv, login, error))
	{
    MakeOrderReject(*pCsv, error);
		CClientsManager::getInstance().Send(login, *pCsv, error);
		return rc;
	}

  CRomOrder* pOrder = NULL;
  std::string routeID, destID;
  pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE, destID);
  CDestsManager::getInstance().GetRouteIdFromDestId(destID, routeID);
  pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, routeID);

	pOrder = m_pOrders->CreateOrder(*pCsv);

	if (CRomOrder::IsCurrencyType(pOrder->SecType()) == true)
	{
		std::string symbol = pOrder->Symbol();
		pOrder->SetCurrency((symbol.substr(0, 3)).c_str());
	}
	else
	{
		pOrder->SetCurrency("USD");
	}

	pOrder->SetCurrencyRate(1);
	pOrder->SetDest(routeID.c_str());

  COats::tMsgList oatsMsgs;
	std::string omExTag;
	pOrder->NextRomExecTag(omExTag);
	pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
  COatsMsg* pMsg = new COatsMsg(pCsv, pOrder, false);
	oatsMsgs.push_back(pMsg);
	rc = false;

  std::list<CRomOrder*> childList;
	if (pCsv->HasChildren() == true)
	{
		const CCsv::tChildren& children = pCsv->GetChildren();
		CCsv::tChildrenIterator it = const_cast<CCsv::tChildren&>(children).begin();
		CCsv* pM = NULL;
		CRomOrder* pChild = NULL;
    std::string parentRomTag = pOrder->RomTag();

		while (it != children.end())
		{
			pM = *it;
      std::string MSG;
      pM->GetMsg(MSG);
			CCsv childCsv(MSG);
      childCsv.SetAt(ROMFIELDS::CSV_OWNER, pOrder->Owner());
			childCsv.SetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, pOrder->RomTag());
			childCsv.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, ROMVALUE::CPLXTYPE_LEG);

			pChild = m_pOrders->CreateOrder(childCsv);
			pM->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, pChild->RomTag());

      pOrder = m_pOrders->FindOrderFromRomTag(parentRomTag.c_str());
			pChild->InheritParent(pOrder);
			childList.push_back(pChild);

			pChild->NextRomExecTag(omExTag);
			childCsv.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
			COatsMsg* pMsg = new COatsMsg(&childCsv, pChild, true);
			oatsMsgs.push_back(pMsg);
			it++;
		}
	}
	else 
	{
		pOrder->SetCplxType(ROMVALUE::CPLXTYPE_PRODUCT_SPREAD);
	}

 	if (CDestsManager::getInstance().Send(routeID, *pCsv, error) == false)
	{
    MakeOrderReject(*pCsv, error);
    CClientsManager::getInstance().Send(login, *pCsv, error);

    COats::tMsgListIt it = oatsMsgs.begin();
    COatsMsg* pOatsMsg = NULL;

    while(oatsMsgs.empty())
    {
      pOatsMsg = *it;
      delete pOatsMsg;
      it = oatsMsgs.erase(it);
    }

    m_pOrders->DestroyOrder(pOrder);
    std::list<CRomOrder*>::iterator it1 = childList.begin();
    while (it1 != childList.end())
    {
      pOrder = *it1;
      m_pOrders->DestroyOrder(pOrder);
      it1 = childList.erase(it1);
    }
  }
  else
  {
    if (oatsMsgs.size() > 0)
	  {
		  DoOats(oatsMsgs);
	  }
  }

	return rc;
}


bool COrderHandler::ProcessClientCancel(CCsv* pCsv)
{
  bool rc = false;
  std::string tag, owner;
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
  pCsv->GetAt(ROMFIELDS::CSV_TAG, tag);
  CRomOrder theOrder;
  if (
  CRomOrder* pOrder = m_pOrders->FindOrderFromClientTag(owner + tag);
  if (!pOrder)
  {
    std::string romtag;
    pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
    pOrder = m_pOrders->FindOrderFromRomTag(romtag.c_str());
  }

  std::string err = "";

  if (!pOrder)
  {
    err = "Order not found";
		pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
		pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
		pCsv->SetAt(ROMFIELDS::CSV_TEXT, err);
		//RomLog::LogMessage(err, msg.Message());
    CClientsManager::getInstance().Send(owner, *pCsv, err);
    rc = true;
	}
  else
  {
    std::string romExTag;
    std::list<COatsMsg*> oatsMsgs;

    CancelAddRequiredFields(*pCsv, *pOrder);
    pOrder->NextRomExecTag(romExTag);

    if (CDestsManager::getInstance().Send(pOrder->Dest(), *pCsv, err))
    {
      pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
			pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
      std::string newtag;
      pCsv->GetAt(ROMFIELDS::CSV_NEW_TAG, newtag);
			pOrder->SetActClientTag(newtag.c_str());
      pOrder->SetStatus(ROMVALUE::STATUS_ROMCANCELPENDING);
    }
    else
    {
      MakeCancelReject(*pCsv, *pOrder, err);
      std::string owner;
      pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);
      CClientsManager::getInstance().Send(owner, *pCsv, err);
    }

    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false);
		oatsMsgs.push_back(pOatsMsg);
    DoOats(oatsMsgs);
  }

	return rc;
}


bool COrderHandler::ProcessClientCancels(CCsv* pCsv)
{
  std::list<COatsMsg*> oatsMsgs;
	m_pOrders->Cancels(*pCsv, oatsMsgs);
  if (oatsMsgs.size() > 0)
  {
    DoOats(oatsMsgs);
  }
  return true;
}

bool COrderHandler::ProcessClientReplace(CCsv* pCsv)
{
  bool rc = false;
  std::string tag, romtag, owner, err;
  pCsv->GetAt(ROMFIELDS::CSV_TAG, tag);
  pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG, romtag);
  pCsv->GetAt(ROMFIELDS::CSV_OWNER, owner);

  CRomOrder* pOrder = m_pOrders->FindOrderFromClientTag(owner + tag);
  if (!pOrder)
  {
    pOrder = m_pOrders->FindOrderFromRomTag(romtag.c_str());
  }

  if (!pOrder)
  {
    err = "Order not found";
		pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
		pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
		pCsv->SetAt(ROMFIELDS::CSV_TEXT, err);
		//RomLog::LogMessage(err, msg.Message());
    CClientsManager::getInstance().Send(owner, *pCsv, err);
    rc = true;
	}
  else
  {
    std::string err;
    std::string omExTag;
		long leave = 0;
    pCsv->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leave);
    bool bFixClient = (leave == -1)? true : false;

    pOrder->NextRomExecTag(omExTag);
    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

    if (pOrder->ReplacePending() == false)
    {
      pOrder->UpdateReplacingFields(*pCsv, bFixClient);
      ReplaceAddRequiredFields(*pCsv, *pOrder);
      if (pOrder->ShareChange() + pOrder->Leave() > 0)
      {
        if (CDestsManager::getInstance().Send(pOrder->Dest(), *pCsv, err))
        {
          pOrder->SetReplacePending(true);
          std::string newtag;

          if (pCsv->GetAt(ROMFIELDS::CSV_NEW_TAG, newtag) == true)
					{
						pOrder->SetNewClientTag(newtag.c_str());
					}
          pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
					pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);
        }
        else
        {
          MakeReplaceReject(*pCsv, err);
          CClientsManager::getInstance().Send(owner, *pCsv, err);
        }
      }
      else
      {
        char text[100];
				sprintf(text, "New Order Share has to be larger than current Cum(%ld)", pOrder->Cum());
				MakeReplaceReject(*pCsv, text);
        CClientsManager::getInstance().Send(owner, *pCsv, err);
      }
    }
    else
    {
      err = "Previous Replace is still pending";
      MakeReplaceReject(*pCsv, err);
    }

    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
    COatsMsg* pOatsMsg = new COatsMsg(pCsv, pOrder, false);
		DoOats(pOatsMsg);
  }

	return rc;
}

void COrderHandler::AddOrder(CRomOrder* pOrder)
{
	m_pOrders->AddOrder(pOrder);
}

void COrderHandler::MakeOrderReject(CCsv& msg, const std::string& reason)
{
  msg.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, " ");
	msg.SetAt(ROMFIELDS::CSV_AVG_PRICE, "0");
	msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, "0");
	msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, "0");
	
	msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
	msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
	msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void COrderHandler::MakeCancelReject(CCsv& msg, const CRomOrder& Order, const std::string& reason)
{
  msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
	msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
	//msg.SetAt(ROMFIELDS::CSV_NEW_TAG, Order.ActClientTag());
	msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void COrderHandler::MakeReplaceReject(CCsv& msg, const std::string& reason)
{
  msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
	msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
	msg.SetAt(ROMFIELDS::CSV_TEXT, reason);
}

void COrderHandler::ReplaceAddRequiredFields(CCsv& msg, const CRomOrder& Order)
{
  msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, Order.RomTag());
  msg.SetAt(ROMFIELDS::CSV_SHARES, Order.Leave() + Order.ShareChange());
	msg.SetAt(ROMFIELDS::CSV_SYMBOL, Order.Symbol());
	msg.SetAt(ROMFIELDS::CSV_CAPACITY, Order.Capacity());
	msg.SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, Order.Cum() + Order.Leave() + Order.ShareChange());
  msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, Order.Leave());
	msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, Order.Cum());
	msg.SetAt(ROMFIELDS::CSV_SIDE, Order.Side());
}

void COrderHandler::CancelAddRequiredFields(CCsv& msg, const CRomOrder& Order)
{
  msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, Order.RomTag());
	msg.SetAt(ROMFIELDS::CSV_SIDE, Order.Side());
	msg.SetAt(ROMFIELDS::CSV_SHARES, Order.Shares());
	msg.SetAt(ROMFIELDS::CSV_SYMBOL, Order.Symbol());
}

bool COrderHandler::ValidateOrder(const CCsv& order, const std::string& login, std::string& error)
{
  //make sure order tag not duplicated
	std::string tag;
  order.GetAt(ROMFIELDS::CSV_TAG, tag);
	if (m_pOrders->FindOrderFromClientTag((login + tag).c_str()))
	{
		error = "Duplicated order tag:" + tag;
		return false;
	}

	//Short lender
	short side = 0;
  order.GetAt(ROMFIELDS::CSV_SIDE, side);
	if ((side == ROMVALUE::SIDE_SHORT || side == ROMVALUE::SIDE_EXEMPT) &&
		  order.IsFieldEmpty(ROMFIELDS::CSV_SHORT_LENDER))
	{
		error = "Short Lender(tag 58) can not be empty for short sell or short sell exempt order";
		return false;
	}

  //clearing id
	if (order.IsFieldEmpty(ROMFIELDS::CSV_CLEARINGID))
	{
		error = "ClearingID(tag 11) can not be empty";
		return false;
	}

	//validate routeable exchange code
  std::string destRtId, exchange;
  order.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange);
  if (CDestsManager::getInstance().GetRouteIdFromDestId(exchange, destRtId) == false)
	//if (-1 == destRtId)
	{
		error = "Invalid Exchange(tag 13)";
		return false;
	}

  //capacity checking
  char capacity = 'P', secType = 'E';
  order.GetAt(ROMFIELDS::CSV_CAPACITY, capacity);
  order.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType);
  order.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange);
  CDestsManager::getInstance().GetRouteIdFromDestId(exchange, destRtId);
  CDestsManager::getInstance().GetSecTypeFromRouteId(destRtId, secType);
	bool rc = true;

	if (ROMVALUE::SECTYPE_OPTION == secType &&
		CConfig::getInstance().IsSpecialOptionDest(destRtId) == false)
	{
		if ('B' != capacity &&
			'S' != capacity)
		{
			error = "Option Order is allowed only for Capacity 'S'(MarketMaker) and Capacity 'B'(BrokerDealerFirm)";
			return false;
		}
	}
	else
	{
		if ('A' != capacity)
		{
			error = "NON Option Order is allowed only for Capacity 'A'(Agency)";
			return false;
		}
	}

  return true;
}


void COrderHandler::DoDropCopy(CCsv& msg, CRomOrder* pOrder, CTraderData* pData)
{
	std::string sOwner = pOrder->Owner();
	int status = 0;
  msg.GetAt(ROMFIELDS::CSV_STATUS, status);

	if (!pData)
	{
		//RomLog::LogMessage("DropCopy error  ", "Trader data not found for " + sOwner);
		return;
	}

	if (pData->ReportingFirm().compare("ROMTPOS") == 0 ||
		(pData->ReportingFirm()).empty())//since current db has this value
	{
		return;
	}

	switch (pData->ReportingType())
	{
	case ROMVALUE::DROPCOPY_FILLONLY:
		if (status != ROMVALUE::STATUS_FILLED &&
			  status != ROMVALUE::STATUS_PARTIALLYFILLED)
		{
			 return;
		}

		break;
	case ROMVALUE::DROPCOPY_ALL:
		break;
	default:
		return;
	}

	msg.SetAt(ROMFIELDS::CSV_CLEARINGID, pOrder->ClearingId());
	msg.SetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, pOrder->ClearingAccount());

	if (strlen(pOrder->ClientEcho()))
	{
		msg.SetAt(ROMFIELDS::CSV_CLIENT_ECHO, pOrder->ClientEcho());
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
	{
		msg.SetAt(ROMFIELDS::CSV_UNDERLYING, pOrder->Underlying());
	}


	if (ROMVALUE::STATUS_FILLED == status ||
		  ROMVALUE::STATUS_PARTIALLYFILLED == status)
	{
    std::string text = GetDropCopyText(pOrder, msg);
		msg.SetAt(ROMFIELDS::CSV_TEXT, text);
	}

	char value[200];
	sprintf(value, 
			    "%s %s %s %s", 
			    pOrder->TradeFor(),
			    pOrder->ClearingAccount(),
			    pOrder->Firm(),
			    pOrder->LocalAccount());

	std::string acct;
  msg.GetAt(ROMFIELDS::CSV_ACCOUNT, acct);
	msg.SetAt(ROMFIELDS::CSV_ACCOUNT, value);

  std::string err;

  if (CClientsManager::getInstance().Send(pData->ReportingFirm(), msg, err))
	{
		//RomLog::LogMessage("DropCopy To " + pData->ReportingFirm(), msg.Message());
	}
	else
	{
		//RomLog::LogMessage("Can not send DropCopy to " + pData->ReportingFirm(), msg.Message());
	}

	msg.SetAt(ROMFIELDS::CSV_ACCOUNT, acct);
}



void COrderHandler::UpdateTraderBalance(const CCsv& msg, CRomOrder* pOrder)//, CTraderData* pData)
{
	short newstatus = 0;
  msg.GetAt(ROMFIELDS::CSV_STATUS, newstatus);
	tBalanceUpdate update;
  memset(&update, 0, sizeof(tBalanceUpdate));

  CUtility::StrCpy(update.trader, sizeof(update.trader), pOrder->Trader());
  update.side = pOrder->Side();
  bool doupdate = false;

	if (newstatus == ROMVALUE::STATUS_FILLED ||
		  newstatus == ROMVALUE::STATUS_PARTIALLYFILLED)
	{
		double price = 0;
    msg.GetAt(ROMFIELDS::CSV_LAST_PRICE, price);
		long lastshares = 0;
    msg.GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshares);
    doupdate = true;

    if (strlen(pOrder->ParentRomTag()) == 0) //cplx order legs are not included in limit checking!!
    {
			if (CRomOrder::IsLimitType(pOrder->Type()))
			{
				update.dollarChange = -1 * lastshares * pOrder->Price() * pOrder->Multiplier() * pOrder->CurrencyRate();
			}
			//m_pClientProfiles->UpdateTraderBalance(pOrder->Trader(), -1 * lastshares, valueChange, 0, pData);
      update.shareChange = -lastshares;
      if (newstatus == ROMVALUE::STATUS_FILLED)
      {
        update.orderChange = -1;
      }
    }
	}
	else if (newstatus == ROMVALUE::STATUS_REPLACED)
	{
		if (strlen(pOrder->ParentRomTag()) == 0) //cplx order is not included in limit checking!!
    {
			if (CRomOrder::IsLimitType(pOrder->Type()))
			{
				update.dollarChange = pOrder->Multiplier() * pOrder->CurrencyRate();
				if (pOrder->PriceChange() != 0 && pOrder->ShareChange() != 0)
				{
					update.dollarChange *= pOrder->PriceChange() * (pOrder->Leave() + pOrder->ShareChange()) + pOrder->ShareChange() * pOrder->Price();
				}
				else if (pOrder->PriceChange() != 0 && pOrder->ShareChange() == 0)
				{
					update.dollarChange *= pOrder->PriceChange() * pOrder->Leave();
				}
				else if (pOrder->PriceChange() == 0 && pOrder->ShareChange() != 0)
				{
					update.dollarChange *= pOrder->ShareChange() * pOrder->Price();
				}
				else
				{
					update.dollarChange = 0;
				}
			}
      doupdate = true;
      update.shareChange = pOrder->ShareChange();
    }
	}
	else if (newstatus == ROMVALUE::STATUS_CANCELED ||
			     newstatus == ROMVALUE::STATUS_EXPIRED ||
			     newstatus == ROMVALUE::STATUS_ORDERREJECTED)
	{
    doupdate = true;
		if (CRomOrder::IsLimitType(pOrder->Type()))
		{
			update.dollarChange =  -1 * pOrder->Price() * pOrder->Leave() * pOrder->Multiplier() * pOrder->CurrencyRate();
		}
    update.shareChange = -1 * pOrder->Leave();
    update.orderChange = -1;
	}

  if (true == doupdate)
  {
    CClientProfileManager::getInstance().AddBalanceUpdate(update);
  }
}

std::string COrderHandler::GetDropCopyText(const CRomOrder* pOrder, CCsv& msg)
{
  int OrderDest = 0;
  msg.GetAt(ROMFIELDS::CSV_ORDERDEST, OrderDest);
  std::string clearingID;
  msg.GetAt(ROMFIELDS::CSV_CLEARINGID, clearingID);
	std::string Purpose = "";
	std::string Exchange = "";
	std::string FTP = "FTP ";
  std::string destName, exdest;
  char sectype;

  msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype);
  msg.GetAt(ROMFIELDS::CSV_EXDEST, exdest);

  CDestsManager::getInstance().GetDestNameFromDestId(pOrder->Dest(), destName);
	if (CRomOrder::IsOptionType(sectype) != true)
	{
    switch(OrderDest)
		{
			case 0: //ARCA
				if (exdest == "N" ||
					  exdest == "A")
				{
          std::string qtag;
          msg.GetAt(ROMFIELDS::CSV_QUERY_TAG, qtag);
					if (qtag == "DOT")
					{
						Purpose = "FTP SWST";
            Purpose += exdest;
					}
					else
					{
						Purpose = FTP + destName;
					}
				}
				else
				{
					//Purpose = "FTP " + CString(COrder::Destination(OrderDest));
					Purpose = FTP + destName;
				}
				msg.GetAt(ROMFIELDS::CSV_EXDEST, Exchange);
		
				break;
			case 6: //Globex
				Exchange = "GLOBEX";
				Purpose = FTP + Exchange;
				break;
            case 8: //ESP
                
                if(clearingID == "0690" ||
                    clearingID == "690") 
				{
					msg.SetAt(ROMFIELDS::CSV_CONTRA, "NET");
                } 
                Purpose = "FTP ESP";
 
                break;
			
			default:
				Purpose = FTP + destName;
				Exchange = destName;
				break;
		}
	}
	else
	{
		std::string capacity;
    msg.GetAt(ROMFIELDS::CSV_CAPACITY, capacity);
		Purpose = "FTP ";
		Purpose += destName;
		
		Purpose += " ";
    int nOpenClose = 0;
    msg.GetAt(ROMFIELDS::CSV_OPENCLOSE, nOpenClose);
		if (nOpenClose == 1)
		{
			Purpose += "O ";
		}
		else
		{
			Purpose += "C ";
		}

		if (capacity == ROMFIELDS::ROM_BROKER_DEALER ||
			capacity == ROMFIELDS::ROM_BROKER_DEALER_FIRM)
		{
			Purpose += "BD";
		}
		else if (capacity == ROMFIELDS::ROM_MARKET_MAKER ||
				 capacity == ROMFIELDS::ROM_MARKET_MAKER_FIRM)
		{
			Purpose += "MM";
		}
		else
		{
			Purpose += "CU";
		}

    if (exdest == "ISE")
		{
			Exchange = "I";
		}
		else if (exdest == "AMEX")
		{
			Exchange = "A";
		}
		else if (exdest == "CBOE")
		{
			Exchange = "C";
		}
		else if (exdest == "PCX")
		{
			Exchange = "P";
		}
		else if (exdest == "PHLX")
		{
			Exchange = "W";
		}
		else if (exdest == "BOX")
		{
			Exchange = "B";
        } 
		else 
		{
           	Exchange = destName;
        }
	}

  int nExchange = 0;
  msg.GetAt(ROMFIELDS::CSV_EXCHANGE, nExchange);
	if (nExchange == 0)
	{
    std::string session;
    msg.GetAt(ROMFIELDS::CSV_TRADING_SESSION, session);
		if (session == "P")
		{
			Exchange = "P";
		}
	}

	if (Exchange == "Direct+")
	{
		Exchange = "N+";
	}

	std::string seprator = ":";
  std::string contra;
  msg.GetAt(ROMFIELDS::CSV_CONTRA, contra);
	return  Purpose + seprator + contra + seprator + Exchange;
}