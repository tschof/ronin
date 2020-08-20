#ifndef __OATS_H__
#define __OATS_H__

#include "ace/Task.h"

class COats : public ACE_Task<ACE_MT_SYNCH>
{
public:

	static COats & getInstance()
	{
		static COats Oats;
		return Oats;
	}

	void AddMsg(ACE_Message_Block* pMsg);

	COats();
	~COats();
};

#endif //__OATS_H__
