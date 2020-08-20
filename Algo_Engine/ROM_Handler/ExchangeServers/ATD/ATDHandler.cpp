// ATDHandler.cpp: implementation of the CATDHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ATDHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CATDHandler::CATDHandler()
{
	InitializeMemoryMap();
	InitializeOrderValidator();
}

void CATDHandler::InitializeMemoryMap()
{
	CString exchangeDestinationId;

	CString dbUser;
	CString dbPassword;
	CString	dbServer;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();

	GetPrivateProfileString(pApp->Name() + " Server",
							"Exchange Destination Id",
							"",
							exchangeDestinationId.GetBuffer(128),
							128,
							pApp->DatFile());

	exchangeDestinationId.ReleaseBuffer();

	GetPrivateProfileString("Order Database Information",
							"User",
							"",
							dbUser.GetBuffer(128),
							128,
							pApp->DatFile());

	dbUser.ReleaseBuffer();

	GetPrivateProfileString("Order Database Information",
							"Password",
							"",
							dbPassword.GetBuffer(128),
							128,
							pApp->DatFile());

	dbUser.ReleaseBuffer();

	GetPrivateProfileString("Order Database Information",
							"Server",
							"",
							dbServer.GetBuffer(128),
							128,
							pApp->DatFile());

	dbServer.ReleaseBuffer();

	bool forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload", 0, pApp->DatFile()) == 0 ? false : true;

	TIDatabaseSettingsAutoPtr lDatabaseSettings = CDatabaseSettingsFactory::CreateDatabaseSettings();

	lDatabaseSettings->AppName((LPCSTR) pApp->Name());
	lDatabaseSettings->DestinationId((LPCSTR) exchangeDestinationId);
	lDatabaseSettings->Server((LPCSTR) dbServer);
	lDatabaseSettings->User((LPCSTR) dbUser);
	lDatabaseSettings->Password((LPCSTR) dbPassword);

	m_pTags = new COptionMemTagSet(lDatabaseSettings, forceReload);

	WritePrivateProfileString("MemoryMapSettings", "ForceReload", "0", pApp->DatFile());
}

void CATDHandler::InitializeOrderValidator()
{
	std::string datFile;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();
	datFile = (LPCSTR) pApp->DatFile();

	m_orderTypeValidator = new COrderTypeValidator(datFile);
}