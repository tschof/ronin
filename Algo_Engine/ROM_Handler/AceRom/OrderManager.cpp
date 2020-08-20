#include "OrderManager.h"
#include "ClientProfileManager.h"
#include <string>


COrderManager::COrderManager()
{
	m_ClientTagToRomOrderNumMap.open();
	InitHandlers();
	InitOrderNum();
}


COrderManager::~COrderManager()
{
}

void COrderManager::InitHandlers()
{
	for (int i = 0; i < HANDLERS_SIZE; i++)
	{
		m_pHandlers[i] = new COrderHandler(i);
		m_pHandlers[i]->activate();
	}
}

void COrderManager::GetNextRomTag(const std::string& owner, std::string& tag)
{
}

bool COrderManager::AddApprovedOrder(CRomCsv* pCsv)
{
	unsigned __int64 ordNum = GetOrderNumFromRomTag(pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG));
	return m_pHandlers[ordNum % HANDLERS_SIZE]->AddMsg(pCsv);
}

bool COrderManager::AddOrderRequest(CRomCsv* pCsv)
{
	bool rc = true;
	std::string romtag;
	GetNextRomTag(pCsv->Get(ROMFIELDS::CSV_OWNER), romtag);
	ACE_UINT64 ordnum = GetOrderNumFromRomTag(romtag);
	m_ClientTagToRomOrderNumMap.bind(pCsv->Get(ROMFIELDS::CSV_TAG), ordnum);
	//send the order to clientprofile manager for approve
	CClientProfileManager::getInstance().AddOrderRequest(pCsv);
	return rc;
}

bool COrderManager::AddReplaceOrCancel(CRomCsv* pCsv)
{
	bool rc = false;
	char tag[128];

	if (pCsv->GetAt(ROMFIELDS::CSV_TAG).empty() == false)
	{
		unsigned __int64 ordNum = GetOrderNumThroughClientTag(pCsv->Get(ROMFIELDS::CSV_TAG));
		rc = m_pHandlers[ordNum % HANDLERS_SIZE]->AddMsg(pCsv);
	}

	return rc;
}

bool COrderManager::AddCancels(CRomCsv* pCsv)
{
	for (int i = 0; i < HANDLERS_SIZE; i++)
	{
		m_pHandlers[i]->AddMsg(pCsv);
	}

	return true;
}


bool COrderManager::AddClientRequest(CRomCsv* pCsv)
{
	bool rc = false;
	
	switch(pCsv->cGet(ROMFIELDS::CSV_COMMAND))
	{
	case ROMVALUE::COMMAND_ORDER:
		rc = AddOrderRequest(pCsv);
		break;
	case ROMVALUE::COMMAND_CANCEL:
	case ROMVALUE::COMMAND_REPLACE:
		rc = AddReplaceOrCancel(pCsv);
		break;
	case ROMVALUE::COMMAND_CANCELS:
		//will need to submit the request to all handlers!!!
		rc = AddCancels(pCsv);
		break;
	default:
		break;
	}

	return rc;
}

	
bool COrderManager::AddHostUpdate(CRomCsv* pCsv)
{
	unsigned __int64 ordNum = GetOrderNumFromRomTag(pCsv->GetAt(ROMFIELDS::CSV_STAFFORD_TAG));
	return m_pHandlers[ordNum % HANDLERS_SIZE]->AddMsg(pCsv);
}

unsigned __int64 COrderManager::GetNextOrderNum()
{
	return m_OrderNum++;
}

unsigned __int64 COrderManager::GetOrderNumFromRomTag(const std::string& tag)
{
	unsigned __int64 rc = 0;
	int index = tag.find_last_of('-');

	if (index != std::string::npos &&
		index + 1 < tag.size())
	{
		std::string ordNumStr = tag.substr(index + 1);
		rc = _atoi64(ordNumStr.c_str());
	}

	return rc;
}

unsigned __int64 COrderManager::GetOrderNumThroughClientTag(const std::string& tag)
{
	unsigned __int64 rc = 0;

	tStringToUINT63Map::iterator endIt = m_ClientTagToRomOrderNumMap.end();
	tStringToUINT63Map::iterator it = endIt;
	m_ClientTagToRomOrderNumMap.find(tag.c_str(), it);

	if (it != endIt)
	{
		rc = it->item();
	}

	return rc;
}


void COrderManager::InitOrderNum()
{
	if (CConfig::getInstance().IsFirstRun())
	{
		m_OrderNum = 0;
	}
	else
	{
		ACE_Time_Value tv = ACE_OS::gettimeofday();
		ACE_UINT64 usec;
		tv.to_usec(m_OrderNum);
		m_OrderNum += 1000000;
	}
}

void COrderManager::LoadOrdersFromDB()
{
}
