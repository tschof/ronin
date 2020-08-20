#include "StdAfx.h"
#include "Config.h"
#include "Mailer.h"
//#include <time.h>
//#include <iomanip>

CConfig::CConfig()
{
	char buf[256];
	GetModuleFileName(NULL, buf, 256);
	m_HomeDir = buf;
	std::string consoleTitle(buf), version = "2.8.0";

	int index = m_HomeDir.find_last_of('\\');
	m_HomeDir.replace(index + 1, 1, 1, '\0');

	std::stringstream stm;
	stm << m_HomeDir.c_str() << "Oats.dat";
	m_datFile	= stm.str();
	
	std::stringstream stm1;
	stm1 << m_HomeDir.c_str() << "Seq.map";
	m_SeqMapFile = stm1.str();
	
	Load();

    //GetPrivateProfileString("Runtime Information",
	//	                    "Server Version", "1.0.0", (char*) version.c_str(), 128,
	//	                    m_datFile.c_str());

	consoleTitle += version.c_str();
	SetConsoleTitle(consoleTitle.c_str());

	QueryPerformanceFrequency(&m_PFFrequency);
}

void CConfig::SaveConfig(const std::string& sectionName, const std::string& name, const std::string& value)
{
	WritePrivateProfileString(sectionName.c_str(), name.c_str(), value.c_str(), m_datFile.c_str());
}

void CConfig::GetConfig(const std::string& sectionName, const std::string& name, std::string& value)
{
	char buffer[128];
	GetPrivateProfileString(sectionName.c_str(), name.c_str(), "", buffer, 128,  m_datFile.c_str());
	value = buffer;
}


void CConfig::Load()
{
	char buf[256];

	GetPrivateProfileString("Database Information", "DSN", "", buf, 256, m_datFile.c_str()); 
	m_dbServer = buf;

	GetPrivateProfileString("Database Information", "User", "", buf, 256, m_datFile.c_str()); 
	m_dbLogin = buf;

	GetPrivateProfileString("Database Information", "Password", "", buf, 256, m_datFile.c_str()); 
	m_dbPassword = buf;

	GetPrivateProfileString("Database Information", "Database", "ROM", buf, 256, m_datFile.c_str()); 
	m_Database = buf;

	m_nDBThreads = GetPrivateProfileInt("Database Information", "ThreadNum", 1, m_datFile.c_str()); 
	m_MaxOatsFileSize = GetPrivateProfileInt("Settings", "Max CSVFile Size", 50000000, m_datFile.c_str()); 

    GetPrivateProfileString("Settings", "Mail Server", "sron-chi-exc01.ronin.roncap.com", buf, 256, m_datFile.c_str()); 
	m_MailServer = buf;

    GetPrivateProfileString("Settings", "Mail Recipients", "ROMBusAlerts@ronin-capital.com", buf, 256, m_datFile.c_str()); 
	m_MailRecipients = buf;

	//GetPrivateProfileString("Runtime Information", "Last Run Date", "", buf, 256, m_datFile.c_str()); 
	//m_LastRunDate = buf;

	GetPrivateProfileString("Settings", "WorkPath", "C:\\", buf, 256, m_datFile.c_str());
	m_Drive = buf;

	GetPrivateProfileString("OM Server Information", "IP", "LocalHost", buf, 256, m_datFile.c_str());
	m_RomServerIP = buf;

	GetPrivateProfileString("Admin Server Information", "IP", "LocalHost", buf, 256, m_datFile.c_str());
	m_AdminServerIP = buf;

	GetPrivateProfileString("Settings", "MSMQ BaseName", "OatsQ", buf, 256, m_datFile.c_str());
	m_MSMSQBaseName = buf;

	m_RomServerPort = GetPrivateProfileInt("OM Server Information", "PORT", 4400, m_datFile.c_str()); 
	m_AdminServerPort = GetPrivateProfileInt("Admin Server Information", "PORT", 4100, m_datFile.c_str()); 

	m_DbPfmLogOn = GetPrivateProfileInt("Settings", "DB Performance Log On", 0, m_datFile.c_str()); 

	m_OrderNumWithRomInstance = GetPrivateProfileInt("Settings", "Using ROM Instance", 0, m_datFile.c_str()); 

	m_dbEnabled = GetPrivateProfileInt("Database Information", "Enable", 1, m_datFile.c_str()); 

	m_numMsgsPerQueuing = GetPrivateProfileInt("Recover", "Messages per Queuing", 2500, m_datFile.c_str()); 
	m_VmUpLimit = GetPrivateProfileInt("Recover", "VM UpLimit", 200, m_datFile.c_str()); 
	m_Sleep = GetPrivateProfileInt("Recover", "Sleep Time", 100, m_datFile.c_str()); 

	m_EndTime = GetPrivateProfileInt("Settings", "End Time", 2000, m_datFile.c_str()); 

	m_WarningQueueRatio = GetPrivateProfileInt("Settings", "Warning Queue Ratio", 2, m_datFile.c_str()); 

	m_WarningQueueSize = GetPrivateProfileInt("Settings", "Warning Queue Size", 100000, m_datFile.c_str()); 

	m_maxClientQueueSize = GetPrivateProfileInt("Settings", "Max Client Queue Size", 50000, m_datFile.c_str()); 

	m_romInstanceOffset = GetPrivateProfileInt("Settings", "RomInstance offset", 8, m_datFile.c_str()); 

	std::string dumSbj = "";
	std::string dumMsg = "";
	SendEmail(dumSbj, dumMsg); //inialize the mail server

	GetPrivateProfileString("Settings", "Sequence Map Obj", "SEQ_MEM", buf, 256, m_datFile.c_str()); 
	m_SeqMapObj = buf;
}

CConfig::~CConfig(void)
{
	//UpdateRunDate();
}

void CConfig::SendEmail(std::string& subject, std::string& msg)
{
    static SMTP::CMailer mailer(m_MailServer, m_MailRecipients, m_MailRecipients);

	if (subject.empty() == false ||
		msg.empty() == false)
	{
		std::string sbj = subject + std::string("<") + MSMSQBaseName() + std::string(">");
		mailer.MailMessage(sbj, msg);
	}
}
/*
void CConfig::UpdateRunDate()
{
	time_t t = time(NULL);
	tm * pDT = localtime( &t );
	std::stringstream stm;
	stm << std::setw(4) << 1900 + pDT->tm_year
		<< std::setw(2) << std::setfill('0') << pDT->tm_mon+1
		<< std::setw(2) << std::setfill('0') << pDT->tm_mday;
	WritePrivateProfileString("Runtime Information", "Last Run Date", stm.str().c_str(), m_datFile.c_str());
	m_FirstRun = false;
}*/

