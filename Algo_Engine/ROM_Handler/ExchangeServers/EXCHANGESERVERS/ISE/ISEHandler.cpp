#include "stdafx.h"
#include "isehandler.h"
#include "isefixsession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef std::vector<std::string> vectorString;

CISEHandler::CISEHandler()
{
	GetPrivateProfileString("Fix Information", "SenderSubID", "", m_senderSubID.GetBuffer(4), 4, m_pApp->DatFile()); 
	m_senderSubID.ReleaseBuffer();
	m_pApp->ProcessDebug("ISE SenderSubID: ",m_senderSubID);
	
	ParseClrFirm2SenderSubIDMap();
	ParseSenderSubIDMap( );
	
	InitializeMemoryMap();
	InitializeOrderValidator();
}

void CISEHandler::ParseClrFirm2SenderSubIDMap()
{
	char  strReturnedString[100];
	DWORD strLen = GetPrivateProfileSection("Clearing Firm SenderSubID Map", strReturnedString, 100, m_pApp->DatFile());
	if(strLen <=0 )
		return;

	char* pSrc = strReturnedString;
	while(pSrc < strReturnedString + strLen)
	{
		char* pNext = strchr(pSrc, NULL );
		if ( pNext != 0 && *pSrc !='#')
		{
			char* pEqual = strchr(pSrc, '=');
			if (pEqual != 0 )
			{
				*pEqual = 0;
				m_clrFirmSenderIDMap[atoi(pSrc)] = pEqual+1;
			}
		}
		pSrc = pNext + 1;
	}
}

void CISEHandler::ParseSenderSubIDMap()
{
	char			strReturnedString[100];
	char			strReturnedString1[50];
	char*			pDest;
	char*			pSrc;
	char*			pTemp;
	int				result = 0;
	int				count = 0;
	vectorString	vectorSenderSubID;

	DWORD strLen = GetPrivateProfileSection("Trader SenderSubID Map", strReturnedString, 100, m_pApp->DatFile());

	if(strLen)
	{
		pSrc = strReturnedString;
		while(result != strLen)
		{
			pDest = strchr( pSrc, NULL );
			result = pDest - strReturnedString + 1;
			if( pDest != NULL )
			{
				if( result <= strLen)
				{
					if(NULL == strchr( pSrc, '#' ))
					{
						if(pTemp = strchr( pSrc, '=' ))
							*pTemp = ' ';
						
						strcpy(strReturnedString1,pSrc);
						vectorSenderSubID.push_back(strReturnedString1);
					}
					pSrc = ++pDest;
				}
			}
			else
				break;
		}
		
		
		while(!vectorSenderSubID.empty())
		{
			
			structSenderSubID lstructSenderSubID;
			vectorString	vectorSenderSubIDParts;

			count = vectorSenderSubID.size() - 1;
			
			ParseSenderSubID( vectorSenderSubID[count--], vectorSenderSubIDParts, ' ' );

			lstructSenderSubID.nOrdersPerSecond = -1;
			strcpy(lstructSenderSubID.strSecondarySenderSubID,"");
			lstructSenderSubID.timestamp = clock();
			lstructSenderSubID.nPrimarySenderSubIDCnt = 0;
			lstructSenderSubID.nSecondarySenderSubIDCnt = 0;					

			switch(vectorSenderSubIDParts.size()-1)
			{
				case 3:
					strcpy(lstructSenderSubID.strSecondarySenderSubID,vectorSenderSubIDParts[3].c_str());	
				case 2:
					lstructSenderSubID.nOrdersPerSecond = atoi(vectorSenderSubIDParts[2].c_str());	
				case 1:
					strcpy(lstructSenderSubID.strSenderSubID,vectorSenderSubIDParts[1].c_str());	
					m_SenderSubIDMap.insert(std::make_pair<CString,structSenderSubID>(vectorSenderSubIDParts[0].c_str(),lstructSenderSubID));
			}

			vectorSenderSubID.pop_back();
		}
	}

}


void CISEHandler::ParseSenderSubID( const std::string & str, std::vector<std::string> & fields, char token, char end  )
{
	std::stringstream stm( str );
	stm.unsetf( std::ios_base::skipws );
	char c;
	std::string s;
	while( !(stm >> c).fail() )
	{
		if( c == token )
		{
			if( s.empty() && !fields.size() )
				continue;
			s = trim(s);
			fields.push_back(s);
			s.erase();
		}
		else if( c == end )
		{
			break;
		}
		else
		{
			s += c;
		}
	}
	fields.push_back(s);
}

std::string CISEHandler::trim( const std::string & str )
{
	LPCTSTR lpsz = str.c_str();
	while (_istspace(*lpsz))
		lpsz = _tcsinc(lpsz);
	return lpsz;
}

void CISEHandler::InitializeMemoryMap()
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

		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER | MODE_ERR_BUSINESS |MODE_ERR_NOTIFY, "ISE Exchange", error);

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

void CISEHandler::InitializeOrderValidator()
{
	std::string datFile;
	CMessageApp *pApp;

	pApp = (CMessageApp *)AfxGetApp();
	datFile = (LPCSTR) pApp->DatFile();

	m_orderTypeValidator = new COrderTypeValidator(datFile);
}


