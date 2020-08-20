#include "StdAfx.h"
#include "Config.h"
#include "shlobj.h"

CConfig::CConfig()
{
	char buf[256];
	GetModuleFileName(NULL, buf, 256);
	m_homePath = buf;

	int index = m_homePath.find_last_of('\\');
	m_homePath.replace(index + 1, 1, 1, '\0');

	std::stringstream stm;
	stm << m_homePath.c_str() << "ROM.dat";
	m_datFile	= stm.str();
	m_instance	= GetPrivateProfileInt("ROM Server", "Instance ID", 2, m_datFile.c_str());

	GetPrivateProfileString("Database Information", "Datasource", "", buf, 256, m_datFile.c_str()); 
	m_dbServer = buf;

	GetPrivateProfileString("Database Information", "User", "", buf, 256, m_datFile.c_str()); 
	m_dbLogin = buf;

	GetPrivateProfileString("Database Information", "Password", "", buf, 256, m_datFile.c_str()); 
	m_dbPassword = buf;

	GetPrivateProfileString("ROM Server", "IP", "", buf, 256, m_datFile.c_str()); 
	m_ServerIP = buf;

	GetPrivateProfileString("Oats Connection", "Oats Recover File Path", "c:\\ordermanager\\", buf, 256, m_datFile.c_str()); 
	m_oatsrecoverfilepath = buf;
	//CreateDirectory(buf, NULL);
	SHCreateDirectoryEx(NULL, buf, NULL);

	GetPrivateProfileString("Sequence Manager", "Client Recover File Path", "c:\\ordermanager\\ClientRecover\\", buf, 256, m_datFile.c_str()); 
	m_clientrecoverfilepath = buf;

	SHCreateDirectoryEx(NULL, buf, NULL);

	GetPrivateProfileString("Sequence Manager", "Client Recover File Path", "c:\\ordermanager\\DestSeq\\", buf, 256, m_datFile.c_str()); 
	m_destSeqFilePath = buf;
	
	//CreateDirectory(buf, NULL);
	SHCreateDirectoryEx(NULL, buf, NULL);

    GetPrivateProfileString("EMAIL", "Mail Server", "sron-chi-exc01.ronin.roncap.com", buf, 256, m_datFile.c_str()); 
	m_MailServer = buf;

    GetPrivateProfileString("EMAIL", "Mail Recipients", "ROMBusAlerts@ronin-capital.com", buf, 256, m_datFile.c_str()); 
	m_MailRecipients = buf;

	GetPrivateProfileString("ROM Server", "Map File", "c:\\ordermanager\\RomMap.file", buf, 256, m_datFile.c_str()); 
	m_MapFile = buf;

	m_ServerPort = GetPrivateProfileInt("ROM Server", "PORT", 8008, m_datFile.c_str()); 

	m_HostCheckPeriod = GetPrivateProfileInt("ROM Server", "Host Check Period", 10, m_datFile.c_str());
	m_ClientCheckPeriod = GetPrivateProfileInt("ROM Server", "Client Check Period", 20, m_datFile.c_str());
    m_SessionCheckPeriod = GetPrivateProfileInt("ROM Server", "Session Check Period", 30, m_datFile.c_str());
	m_OatsEnabled = GetPrivateProfileInt("Oats Connection", "Enabled", 1, m_datFile.c_str());
	m_OatsWarningQueueSize = GetPrivateProfileInt("Oats Connection", "Warning Queue Size", 5000, m_datFile.c_str());
    m_EndTime = GetPrivateProfileInt("ROM Server", "End Time", 1700, m_datFile.c_str());
	//m_EndWDay = GetPrivateProfileInt("ROM Server", "End Week Day", -1, m_datFile.c_str());
	m_LoadOrdersFromDB = GetPrivateProfileInt("Option Information", "Load Orders From DB", 0, m_datFile.c_str()); 
	m_GapFillEnabled = GetPrivateProfileInt("Option Information", "Enable Gap Fill", 1, m_datFile.c_str()); 

	m_clientlogonmaxtry = GetPrivateProfileInt("ROM Client", "Max Client Logon Try", 3, m_datFile.c_str()); 

	char today[9];
	GetPrivateProfileString("ROM Server", "Last Run Date", "", buf, 9, CConfig::getInstance().datFile().c_str()); 
	SYSTEMTIME systime;
    GetSystemTime(&systime);
	wsprintf(today, "%04u%02u%02u", systime.wYear, systime.wMonth, systime.wDay);
	if (strcmp(today, buf) == 0)
	{
		m_firstRun = false;
	}
	else
	{
		m_firstRun = true;
	}
	
	LoadSpecialOptionDests();
	//QueryPerformanceFrequency(&m_PFFrequency);

    char szName[1024];
	GetModuleFileName( NULL, szName, sizeof(szName) );
	std::string consoleTitle(szName), version = "1.0.0";
	
	consoleTitle += version.c_str();
	SetConsoleTitle(consoleTitle.c_str());
}

CConfig::~CConfig(void)
{
}

std::string CConfig::Login()const
{
	char value[10];

	if (m_instance != 2)
	{
		sprintf(value, "ROM%d", m_instance);
	}
	else
	{
		strcpy(value, "ROM");
	}

	std::string rc = value;
	return rc;
}

std::string CConfig::sInstance()const
{ 
	char value[5];
	sprintf(value, "%d", m_instance);
	std::string rc = value;
	return rc; 
}

std::string CConfig::InstanceName()const
{
	std::string rc = "ROM";
	rc.append(sInstance());
	return rc;
}


void CConfig::LoadSpecialOptionDests()
{
	char sNum[5];
	char sBuf[5];
	int i = 0;

	while (1)
	{
		sprintf(sNum, "%d", i++);
		GetPrivateProfileString("Special Option Dests", sNum, "", sBuf, 4, m_datFile.c_str()); 
		
		if (strcmp(sBuf, "") == 0)
		{
			break;
		}
		else
		{
			m_SpecialOptionDests.insert(atoi(sBuf));
		}
	}

	if (m_SpecialOptionDests.empty() == true)
	{
		m_SpecialOptionDests.insert(47); //GLOBEX_O
		m_SpecialOptionDests.insert(79); //ICE_OPT
	}
}

bool CConfig::IsSpecialOptionDest(int destID)
{
	bool rc = false;
	std::set<int>::iterator it = m_SpecialOptionDests.find(destID);
	if (it != m_SpecialOptionDests.end())
	{
		rc = true;
	}
	return rc;
}

void CConfig::LoadReloadables()
{
	LoadSpecialOptionDests();

	m_OatsEnabled = GetPrivateProfileInt("Oats Connection", "Enabled", 1, m_datFile.c_str());
	m_OatsWarningQueueSize = GetPrivateProfileInt("Oats Connection", "Warning Queue Size", 5000, m_datFile.c_str());
    m_EndTime = GetPrivateProfileInt("ROM Server", "End Time", 1700, m_datFile.c_str());
	m_GapFillEnabled = GetPrivateProfileInt("Option Information", "Enable Gap Fill", 1, m_datFile.c_str()); 
}