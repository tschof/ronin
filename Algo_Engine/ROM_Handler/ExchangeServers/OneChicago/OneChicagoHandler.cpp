// OneChicagoHandler.cpp: implementation of the COneChicagoHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OneChicagoHandler.h"
#include "OneChicagoMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COneChicagoHandler::COneChicagoHandler()
{

	GetPrivateProfileString("Special ID", "BranchID", "", m_branchID.GetBuffer(8), 8, m_pApp->DatFile()); 
	m_branchID.ReleaseBuffer();
	m_pApp->ProcessDebug("OneChicago Branch ID: ",m_branchID);

	InitializeMemoryMap();
	InitializeOrderValidator();
}

void COneChicagoHandler::InitializeMemoryMap()
{
	CString exchangeDestinationId;

	CString dbUser;
	CString dbPassword;
	CString	dbServer;
	CMessageApp *pApp;

	try{

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
		
		m_pTags = new COneChicagoMemTagSet(lDatabaseSettings, forceReload, "rom.dbo.XSRV_1C_selDestOrders");

		WritePrivateProfileString("MemoryMapSettings", "ForceReload", "0", pApp->DatFile());
	}
	catch(MMIErrorCodes errorCode)
	{
		CString error = "Unable to intialize memory map";

		switch(errorCode)
		{
		case MMI_DB_CONNECTION_ERROR:
			error += " (Database connection failure)";
			break;
		};

		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS |MODE_ERR_NOTIFY, "OneChicago Exchange", error);

		AfxMessageBox(error, MB_OK | MB_ICONERROR);
		PostQuitMessage(-1);
	}
	catch(...)
	{
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS |MODE_ERR_NOTIFY,m_pApp->Name() +":InitializeMemoryMap", "Failed to Load Memory Map");
		PostQuitMessage(-1);
	}
	
}

void COneChicagoHandler::InitializeOrderValidator()
{
	std::string datFile;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();
	datFile = (LPCSTR) pApp->DatFile();

	m_orderTypeValidator = new COrderTypeValidator(datFile);
}
