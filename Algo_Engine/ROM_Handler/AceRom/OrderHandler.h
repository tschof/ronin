#ifndef __ORDERHANDLER_H__
#define __ORDERHANDLER_H__

#include "ace/Task_T.h"
#include "RomCsv.h"

class COrderHandler : public ACE_Task<ACE_MT_SYNCH>
{
public:

	bool AddMsg(CRomCsv* pCsv);

	COrderHandler(unsigned short id);
	~COrderHandler();
	virtual int svc();

private:

	void ProcessMessage(CRomCsv* pCsv);
};

#endif //__ORDERHANDLER_H__
