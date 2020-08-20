#include "DestsManager.h"

CDestsManager::CDestsManager()
{
}

CDestsManager::~CDestsManager()
{
}

void CDestsManager::OnDestConnect(CDestHandler* pHandler)
{
	//find the login from IP/Port

	//send logon request
	//Notify Oats
}

void CDestsManager::OnDestLogonAck(const std::string& login, CDestHandler* pHandler)
{
	AddHandler(pHandler);
	//Notify Oats thread
}

int CDestsManager::svc()
{
	StartProactorLoop();
	return 0;
}