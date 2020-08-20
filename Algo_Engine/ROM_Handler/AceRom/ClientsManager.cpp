#include "ClientsManager.h"

CClientsManager::CClientsManager(void)
{
}

CClientsManager::~CClientsManager(void)
{
}

bool CClientsManager::AddLogin(const CRomMessage& login, CClientHandler* pHandler)
{
	return true;
}

bool Send(const std::string& receiver, CRomCsv& msg, std::string& err)
{
}

int CClientsManager::svc()
{
	StartProactorLoop();
	return 0;
}
