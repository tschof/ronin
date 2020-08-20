#include "OrderHandler.h"


COrderHandler::COrderHandler(unsigned short id)
{
	
}

COrderHandler::~COrderHandler()
{
}

bool COrderHandler::AddMsg(CRomCsv* pCsv)
{
	bool rc = true;

	ACE_Message_Block* new_mb = NULL;
	ACE_NEW_NORETURN(new_mb, ACE_Message_Block(sizeof(pCsv)));
	memcpy(new_mb->wr_ptr(), &pCsv, sizeof(pCsv));

	if (putq(new_mb) == -1)
	{
		rc = false;
	}

	return rc;
}

int COrderHandler::svc()
{
	ACE_Message_Block *mb = NULL;
	while (this->getq(mb) != -1)
	{
		CRomCsv* pCsv = *((CRomCsv**)(mb->rd_ptr())); 
		ProcessMessage(pCsv);
	}

	return 0;
}


void COrderHandler::ProcessMessage(CRomCsv* pCsv)
{
}
