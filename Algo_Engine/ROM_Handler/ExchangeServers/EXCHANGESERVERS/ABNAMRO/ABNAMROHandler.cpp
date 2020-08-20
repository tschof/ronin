#include "stdafx.h"
#include "ABNAMROhandler.h"
#include "ABNAMROfixsession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CABNAMROHandler::CABNAMROHandler()
{
	GetPrivateProfileString("Fix Information", "SenderSubID", "", m_senderSubID.GetBuffer(6), 6, m_pApp->DatFile()); 
	m_senderSubID.ReleaseBuffer();
	m_pApp->ProcessDebug("ABNAMRO SenderSubID: ",m_senderSubID);
	InitializeMemoryMap();
	InitializeOrderValidator();
}

void CABNAMROHandler::InitializeMemoryMap()
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
		
		m_pTags = new COptionMemTagSet(lDatabaseSettings, forceReload);

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

		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS |MODE_ERR_NOTIFY, "ABNAMRO Exchange", error);

		AfxMessageBox(error, MB_OK | MB_ICONERROR);
		PostQuitMessage(-1);
		//exit(-1);
	}
	catch(...)
	{
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS |MODE_ERR_NOTIFY,m_pApp->Name() +":InitializeMemoryMap", "Failed to Load Memory Map");
		PostQuitMessage(-1);
		//exit(-1);
	}
	
}

void CABNAMROHandler::InitializeOrderValidator()
{
	std::string datFile;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();
	datFile = (LPCSTR) pApp->DatFile();

	m_orderTypeValidator = new COrderTypeValidator(datFile);
}
