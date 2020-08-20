#include "BaseProactor.h"

CBaseProactor::CBaseProactor()
{
	m_hMap.open();
}

CBaseProactor::~CBaseProactor()
{
}


bool CBaseProactor::AddHandler(T* pHandler)
{
	bool rc = false;
	if (m_hMap.bind(pHandler->ID(), pHandler) == 0)
	{
		rc = true;
	}
	return rc;
}


void CBaseProactor::RemoveHandler(T* pHandler)
{
	m_hMap.unbind(pHandler->ID(), pHandler);
}


bool CBaseProactor::SendFromRom(const std::string& handlerID, const std::string& msg)
{
	bool rc = false;
	ACE_Hash_Map_Entry<std::string, T*> entry;
	if (m_hMap.find(handlerID, entry) == 0)
	{
		entry.int_id->Send(msg);
	}
	return rc;
}

bool CBaseProactor::SendToRom(tMsgBlkList& blks)
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



