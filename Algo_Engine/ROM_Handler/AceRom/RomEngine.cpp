#include "RomEngine.h"

CRomEngine::CRomEngine()
{
}

CRomEngine::~CRomEngine()
{
}

void CRomEngine::AddMsg(ACE_Message_Block* pMsg)
{
	putq(pMsg);
}
