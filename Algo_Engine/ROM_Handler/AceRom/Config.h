#ifndef _CONFIG_H__
#define _CONFIG_H__

#include <iostream>
#include <sstream>
#include <set>
//#include "Windows.h"

class CConfig
{
public:
	CConfig();
	~CConfig(void);
	
	static CConfig & getInstance()
	{
		static CConfig theConfig;
		return theConfig;
	}

	const std::string& datFile()const				{ return m_datFile; }
	const std::string& dbServer()const				{ return m_dbServer; }
	const std::string& dbLogin()const				{ return m_dbLogin; }
	const std::string& dbPassword()const			{ return m_dbPassword; }
	std::string Login()const;
	const std::string ServerIP()const				{ return m_ServerIP; }
    std::string sInstance()const;
	std::string InstanceName()const;
    const std::string& OatsRecoverFilePath()const	{ return m_oatsrecoverfilepath; }
	const std::string& ClientRecoverFilePath()const	{ return m_clientrecoverfilepath; }
	const std::string& DestSeqFilePath()const		{ return m_destSeqFilePath;}
    const std::string& MailServer()const			{ return m_MailServer; }
    const std::string& MailRecipients()const		{ return m_MailRecipients; }
	const std::string& MapFile()const				{ return m_MapFile; }
	const std::string& homePath() const				{ return m_homePath; }
	const std::string& CurrentDayStr() const		{ return m_CurrentDayStr; }

    const long HostCheckPeriod()const				{ return m_HostCheckPeriod; }
	const long ClientCheckPeriod()const				{ return m_ClientCheckPeriod; }
    const long SessionCheckPeriod()const			{ return m_SessionCheckPeriod; }
    const long OatsWarningQueueSize()const			{ return m_OatsWarningQueueSize; }
	const short ServerPort()const					{ return m_ServerPort; }
	const short Instance()const						{ return m_instance; }
	const bool OatsEnabled()const					{ return m_OatsEnabled; }
	const long EndTime()const						{ return m_EndTime; }
	const short EndWDay()const						{ return m_EndWDay; }
	const short ClientLogonMaxTry() const			{ return m_clientlogonmaxtry; }

	bool IsSpecialOptionDest(int destID);
	bool LoadOrdersFromDB()							{ return m_LoadOrdersFromDB; }
	bool GapFillEnabled()							{ return m_GapFillEnabled; }
	bool IsFirstRun()								{ return m_firstRun; }
	void SetGapFillEnabled(bool value)				{ m_GapFillEnabled = value; }

	//LARGE_INTEGER& PFFrequency()					{ return m_PFFrequency; }

	void LoadReloadables();

private:
	void LoadSpecialOptionDests();
	
	std::set<int> m_SpecialOptionDests;

	std::string m_datFile;
	std::string m_dbServer;
	std::string m_dbLogin;
	std::string m_dbPassword;
	std::string m_ServerIP;
	std::string m_homePath;
    std::string m_MailServer;
    std::string m_MailRecipients;
	std::string m_oatsrecoverfilepath;
	std::string m_clientrecoverfilepath;
	std::string m_destSeqFilePath;
	std::string m_MapFile;
	std::string m_CurrentDayStr;

    long		m_HostCheckPeriod;
	long		m_ClientCheckPeriod;
    long		m_SessionCheckPeriod;
    long		m_OatsWarningQueueSize;
    long		m_EndTime;
	short		m_EndWDay;
	short		m_ServerPort;
	short		m_instance;
	short		m_clientlogonmaxtry;
	bool		m_OatsEnabled;
	bool		m_LoadOrdersFromDB;
	//LARGE_INTEGER m_PFFrequency;
	bool		m_GapFillEnabled;
	bool		m_firstRun;
};

#endif //_CONFIG_H__
