#include "BaseManager.h"

template<class T>
CBaseManager::CBaseManager()
{
	m_hMap.open();
}

CBaseManager::~CBaseManager()
{
}

bool CBaseManager::AddHandler(T* pHandler)
{
	bool rc = false;
	if (m_hMap.bind(pHandler->ID(), pHandler) == 0)
	{
		rc = true;
	}
	return rc;
}


void CBaseManager::RemoveHandler(T* pHandler)
{
	m_hMap.unbind(pHandler->ID(), pHandler);
	//Notify RomEngine
}


bool CBaseManager::SendFromRom(const std::string& handlerID, const std::string& msg)
{
	bool rc = false;
	ACE_Hash_Map_Entry<std::string, T*> entry;
	if (m_hMap.find(handlerID, entry) == 0)
	{
		entry.int_id->Send(msg);
	}
	return rc;
}

bool CBaseManager::SendToRom(tMsgBlkList& blks)
{
	tMsgBlkListIt it = blks.begin();
	ACE_Message_Block* pBlock = NULL;

	while (it != blks.end())
	{
		pBlock = *it;
		CRomEngine::getInstance().AddMsg(pBlock);
		it++;
	}
}
