#include "DestHandler.h"
#include "OrderManager.h"
#include "DestsManager.h"

CDestHandler::CDestHandler()
{
}

CDestHandler::~CDestHandler()
{
}

void CDestHandler::open(ACE_HANDLE h, ACE_Message_Block& mb)
{
	this->handle(h);

	if (this->m_reader.open(*this) != 0 ||
		this->m_writer.open(*this) != 0)
	{
		ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("CDestHandler.open")));
		delete this;
		return;
	}

	//send logon request
}

void CDestHandler::RemoveHandler()
{
	CDestsManager::getInstance().RemoveHandler(ID());
	//notify oats thread
}

void CDestHandler::ProcessMessage(const char* pMsg, int size)
{
	CRomCsv* pCsv = new CRomCsv(pMsg, size);
	COrderManager::getInstance().AddHostUpdate(pCsv);
}
