#ifndef __BASEPROACTOR_H__
#define __BASEPROACTOR_H__

#include "ace/Proactor.h"
#include <string>

template<class T>
class CBaseProactor : public ACE_Proactor
{
public:
	typedef ACE_Hash_Map_Manager_Ex<std::string, T*, 
									ACE_Hash<std::string>, 
									ACE_Hash_To<std::string>,
									ACE_SYNCH_MUTEX> tHandlerMap;

	CBaseProactor();
	~CBaseProactor();

	//Called by Handler
	virtual bool AddHandler(T* pHandler);
	virtual void RemoveHandler(T* pHandler);

	//Called by the RomEngine
	bool SendFromRom(const std::string& handlerID, const std::string& msg);
	
	//called by handler
	bool SendToRom(ACE_Message_Block* pBlock);

private:

	tHandlerMap m_hMap;
};

#endif //__BASEPROACTOR_H__