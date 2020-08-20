/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLHandler.cpp: implementation of the CMERRILLHandler class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#include "stdafx.h"
#include "MERRILLHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMERRILLHandler::CMERRILLHandler()
{
	InitializeMemoryMap();
	InitializeOrderValidator();
}

void CMERRILLHandler::InitializeMemoryMap()
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

void CMERRILLHandler::InitializeOrderValidator()
{
	std::string datFile;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();
	datFile = (LPCSTR) pApp->DatFile();

	m_orderTypeValidator = new COrderTypeValidator(datFile);
}