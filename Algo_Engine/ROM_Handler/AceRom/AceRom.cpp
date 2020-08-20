// AceRom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Config.h"
#include "OrderManager.h"
#include "Oats.h"
#include "ClientsManager.h"
#include "DestsManager.h"
#include "ClientProfileManager.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CConfig::getInstance();
	CClientProfileManager::getInstance().activate();
	COrderManager::getInstance();
	COats::getInstance().activate();
	CClientsManager::getInstance().activate();
	CDestsManager::getInstance().activate();

	return 0;
}

