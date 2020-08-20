#include "Oats.h"

COats::COats(void)
{
}

COats::~COats(void)
{
}

void COats::AddMsg(ACE_Message_Block* pMsg)
{
	putq(pMsg);
}