// NewOats.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OatsServer.h"
#include "winsock2.h"
#include "Config.h"

#include "Excepthandler.h"

COatsServer* g_pOatsServer = NULL;

BOOL WINAPI ShutdownHandler(DWORD evtType)
{
	BOOL rc = FALSE;

	switch(evtType)
	{
	case CTRL_LOGOFF_EVENT:
		(Logger::getInstance()).LogMessage("Logoff Event Trigged");
		g_pOatsServer->SetLive(false);
		rc = TRUE;
		break;
	case CTRL_SHUTDOWN_EVENT:
		(Logger::getInstance()).LogMessage("Shutdown Event Trigged");
		g_pOatsServer->SetLive(false);
		rc = TRUE;
		break;
	case CTRL_CLOSE_EVENT:
		(Logger::getInstance()).LogMessage("Close Event Trigged");
		g_pOatsServer->SetLive(false);
		rc = TRUE;
		break;
	case CTRL_C_EVENT:
		(Logger::getInstance()).LogMessage("Ctrl-C Event Trigged");
		g_pOatsServer->SetLive(false);
		rc = TRUE;
		break;
	}

	return rc;
}

int _tmain(int argc, _TCHAR* argv[])
{
	CExceptionLogger::Init();
	(CConfig::getInstance()).Init();
	g_pOatsServer = new COatsServer();
	SetConsoleCtrlHandler(ShutdownHandler, TRUE);
	g_pOatsServer->Run();
	delete g_pOatsServer;
	return 0;
}

