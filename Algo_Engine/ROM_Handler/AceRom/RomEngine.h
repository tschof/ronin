#ifndef __ROMENGINE_H__
#define __ROMENGINE_H__

#include "ace/Task.h"

//class CRomEngine : public ACE_Task_Base
class CRomEngine : public ACE_Task<ACE_MT_SYNCH>
{
public:

	static CRomEngine & getInstance()
	{
		static CRomEngine RomEngine;
		return RomEngine;
	}

	void AddMsg(ACE_Message_Block* pMsg);

	CRomEngine();
	~CRomEngine();
};

#endif //__ROMENGINE_H__
