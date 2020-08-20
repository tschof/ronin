#pragma once

#ifndef _CONFIG_H__
#define _CONFIG_H__

#include <iostream>
#include <sstream>

class CConfig
{
public:
	CConfig();
	~CConfig(void);
	
	void Load();

	static CConfig & getInstance()
	{
		static CConfig Config;
		return Config;
	}

	void Init() {}

	const char* DBServer()			{ return m_dbServer.c_str(); }
	const char* DBLogin()			{ return m_dbLogin.c_str(); }
	const char* DBPassword()		{ return m_dbPassword.c_str(); }
	const char* Database()			{ return m_Database.c_str(); }
	//const char* LastRunDate()		{ return m_LastRunDate.c_str(); }
	const char* MailServer()		{ return m_MailServer.c_str(); }
	const char* MailRecipients()	{ return m_MailRecipients.c_str(); }
	const char* Drive()				{ return m_Drive.c_str(); }
	std::string RomServerIP()		{ return m_RomServerIP; }
	std::string AdminServerIP()		{ return m_AdminServerIP; }
	std::string HomeDir()			{ return m_HomeDir; }

	const std::string& MSMSQBaseName() const { return m_MSMSQBaseName; }
	const std::string& SeqMapFile()	const { return m_SeqMapFile; }
	const std::string& SeqMapObj() const { return m_SeqMapObj; }

	void SaveConfig(const std::string& sectionName, const std::string& name, const std::string& value);
	void GetConfig(const std::string& sectionName, const std::string& name, std::string& value);

	short RomServerPort()			{ return m_RomServerPort; }
	short AdminServerPort()			{ return m_AdminServerPort; }
	short numDBThreads()			{ return m_nDBThreads; }
	long MaxOatsFileSize()			{ return m_MaxOatsFileSize; }
	bool OrderNumWithRomInstance()  { return m_OrderNumWithRomInstance; }
	bool DbPfmLogOn()				{ return m_DbPfmLogOn; }
	bool dbEnabled()				{ return m_dbEnabled; }
	LARGE_INTEGER PFFrequency()		{ return m_PFFrequency;}

	//Recover
	int Sleep()			{ return m_Sleep; }
	int numMsgsPerQueuing() { return m_numMsgsPerQueuing;}
	int VmUpLimit()			{ return m_VmUpLimit; }
	unsigned short	EndTime()	{ return m_EndTime; }
	unsigned short WarningQueueRatio() { return m_WarningQueueRatio; }
	unsigned long WarningQueueSize() { return m_WarningQueueSize; }
	void SendEmail(std::string& subject, std::string& msg);
	unsigned long MaxClientQueueSize() { return m_maxClientQueueSize; }
	
	short romInstanceOffset()			{ return m_romInstanceOffset; }

private:
	std::string m_datFile;
	std::string m_dbServer;
	std::string m_dbLogin;
	std::string m_dbPassword;
	std::string m_Database;
	std::string m_HomeDir;

	//std::string m_LastRunDate;
    std::string m_MailServer;
    std::string m_MailRecipients;
	std::string m_Drive;
	std::string m_RomServerIP;
	std::string m_AdminServerIP;
	std::string m_MSMSQBaseName;
	std::string m_SeqMapFile;
	std::string m_SeqMapObj;
	//bool			m_FirstRun;
	short			m_RomServerPort;
	short			m_AdminServerPort;
	short			m_nDBThreads;
	unsigned short	m_EndTime;
	long			m_MaxOatsFileSize;
	unsigned short	m_WarningQueueRatio;
	unsigned long	m_WarningQueueSize;
	bool			m_DbPfmLogOn;
	bool			m_OrderNumWithRomInstance;
	bool			m_dbEnabled;
	unsigned long	m_maxClientQueueSize;
	short			m_romInstanceOffset;//the offset used to find rom instance in romTag
	LARGE_INTEGER	m_PFFrequency;

	//Recover parameters
	int m_Sleep;  //in MS
	int m_numMsgsPerQueuing; 
	int m_VmUpLimit; //distribute messages to db thread only when the process VM below this value(in MB)

};

#endif //_CONFIG_H__
