#ifndef __BASEMANAGER_H__
#define __BASEMANAGER_H__

#include <string>
#include "ace/Message_Block.h"
#include "ace/Task.h"
#include "ace/Hash_Map_Manager_T.h"
#include "ace/Proactor.h"
#include "typedef.h"
#include "BaseHandler.h"
#include "RomCsv.h"

template<class T>
class CBaseManager : public ACE_Task<ACE_MT_SYNCH>
{

public:
	
	typedef ACE_Hash_Map_Manager_Ex<const char*, 
									T*, 
									ACE_Hash<const char*>, 
									ACE_Equal_To<const char*>,
									ACE_SYNCH_MUTEX> tHandlerMap;

	CBaseManager()
	{
		m_hMap.open();
	}

	~CBaseManager()
	{
	}

	bool AddHandler(T* pHandler)
	{
		bool rc = false;
		
		if (m_hMap.bind(pHandler->ID().c_str(), pHandler) == 0)
		{
			rc = true;
		}

		return rc;
	}


	void RemoveHandler(const std::string& handlerID)
	{
		m_hMap.unbind(handlerID.c_str());
	}

	bool Send(const std::string& receiver, CRomCsv& msg, std::string& err)
	{
		bool rc = true;
		tHandlerMap::iterator it = m_hMap.end();
		tHandlerMap::iterator endIt = it;

		m_hMap.find(receiver.c_str(), it);

		if (it != endIt)
		{
			(it->item())->Send(msg);
		}
		else
		{
			rc = false;
			err = std::string("Not connected to ") + receiver;
		}

		return rc;
	}

	void StartProactorLoop()
	{
		m_proactor.proactor_run_event_loop();
	}

/*
	bool SendFromRom(const std::string& handlerID, const std::string& msg)
	{
		bool rc = false;
		tHandlerMap::iterator it = m_hMap.end();
		tHandlerMap::iterator endIt = it;
		m_hMap.find(handlerID.c_str(), it);
		if (it != endIt)
		{
			(it->item())->Send(msg);
		}
		return rc;
	}

	bool SendToRom(tMsgBlkList& blks)
	{
		tMsgBlkListIt it = blks.begin();
		ACE_Message_Block* pBlock = NULL;

		while (it != blks.end())
		{
			pBlock = *it;
			CRomEngine::getInstance().AddMsg(pBlock);
			it++;
		}

		return true;
	}
	*/
private:
	tHandlerMap m_hMap;
	ACE_Proactor m_proactor;
};

#endif //__BASEMANAGER_H__
