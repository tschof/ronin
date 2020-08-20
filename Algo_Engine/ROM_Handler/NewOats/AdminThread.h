#ifndef _ADMINTHREAD_H__
#define _ADMINTHREAD_H__

#pragma once

#include "Windows.h"
#include "Thread.h"
#include "AdminSession.h"
#include "MessageBuffer.h"
#include "ServerSocket.h"
#include "Zlog.h"
#include "SeqMap.h"
#include <map>
#include <list>

static const DWORD SERVER_LISTENING_EVENT_INDEX = 0;
static const DWORD KILL_EVENT_INDEX = 1;
static const DWORD OATS_MESSAGE_EVENT_INDEX = 2;
static const DWORD CHECKDEADSESSION_EVENT_INDEX = 3;
static const DWORD ADMINRESPONSE_MESSAGE_EVENT_INDEX = 4;
static const DWORD MIN_ADMINDISCONNECT_EVENT_INDEX = 5;
static const DWORD MAX_ADMINDISCONNECT_EVENT_INDEX = 40;

class CAdminThread : public Thread
{
public:
	typedef std::map<int, CAdminSession*> tAdminSessionMap;
	typedef tAdminSessionMap::iterator tAdminSessionMapIT;
	typedef std::list<int> tEventIndexList;
	typedef tEventIndexList::iterator tEventIndexListIT;
	typedef std::list<CAdminSession*> tAdminSessionList;
	typedef tAdminSessionList::iterator tAdminSessionListIT;

	typedef std::map<std::string, ZLog*> tLogMap;
	typedef tLogMap::iterator tLogMapIT;;
	typedef std::map<std::string, std::string*> tMsgMap;
	typedef tMsgMap::iterator tMsgMapIT;

	CAdminThread(void);
	~CAdminThread(void);
	HANDLE AdminRquestEvt() { return m_AdminRquestEvt; }// oatsserver will wait for this event
	void SetOatsEvt() { ::SetEvent(m_wsaEvents[OATS_MESSAGE_EVENT_INDEX]); } //used for oatsserver to waikup this thread

	//void ProcessMessage(std::string& l_message);
	void AddMessages(const std::string& msgs);

	//void AddOatsMessages(std::list<CRomCsv*>& msgs);//Called by oatsserve to push in oats message
	void AddAdminResponseMessages(std::list<CRomCsv*>& msgs);//Called by oatsserve to push in admin response message
	void AddAdminResponseMessage(CRomCsv* pCsv);
	void RetrieveAdminRequestMessages(std::list<CRomCsv*>& msgs);//Called by oatsserver to retrieve admin request
	void ProcessMessage(std::string& l_message, CAdminSession* pC); // Process the message from admin
	void Run();
	
private:
	HANDLE m_AdminRquestEvt;

	CMessageBuffer<CRomCsv> m_OatsBuffer; // for oats message from ROM
	CMessageBuffer<CRomCsv> m_AdminResponseBuffer; // for Admin response from ROM
	CMessageBuffer<CRomCsv> m_AdminRequestBuffer; // for Admin request to ROM

	CStringBuffer m_sMsgBuffer;

	tAdminSessionMap m_AdminSessionMap;
	RASL::ServerSocket* m_pServerSock;
	tEventIndexList m_DisconnectEventIndexList;
	WSAEVENT m_wsaEvents[MAX_ADMINDISCONNECT_EVENT_INDEX + 1];
	tAdminSessionList m_DeadAdminSessionList;

	std::string m_datFile;
	//ZLog *m_eLog;
	//ZLog *m_fLog;
	//ZLog *m_oLog;
	//ZLog *m_cLog;
	//ZLog *m_sLog;

	tLogMap m_LogMap;

	SeqMap m_SeqMap;

	void FreeAdminID(int id);
	int CreateAdminID();
	void Shutdown();
	void InitServer();
	//void ProcessOatsMessages(); //called by this thread to retrieve oats messages
	void ProcessAdminResponse(CRomCsv& csv);
	void ProcessAdminResponses(); //called by this thread to retrieve adminresponse messages
	//void ProcessOatsFile(const std::string& l_message);
	void CloseAdminSession(CAdminSession* pC);
	void DeleteDeadAdminSessions();

	void ProcessMessages();
	//void ProcessOatsMessages(std::list<CRomCsv*>& csvList);
	void ProcessOatsMessage(CRomCsv& csv);
	void ProcessAdminResponses(std::list<CRomCsv*>& csvList);
	void LogCsv(const std::string& msgs, char secType, std::string romLogin);
	void LogRecoverCsv(const std::string& msgs, char secType, std::string romLogin , std::string date);
	char GetZLogSecType(CRomCsv& csv);
	void SetCheckTimer(DWORD index, long period);
	void GetRomLoginFromRomTag(const std::string& romTag, std::string& romLogin);
	bool ProcessCsvFullRecover(const std::string& recoverFile);
	bool ProcessCsvPartialRecover(const std::string& recoverFile);
	void CloseCsvFiles();
	
	void GetDateFromRomTag(const std::string& romTag, std::string& date);
	//void GetRomLoginFromRomTag(const std::string& romTag, std::string& romLogin);
	void PreProcessCsvFullRecover(const CRomCsv& csv, std::string& romLogin, std::string& date);
	void PostProcessCsvFullRecover();
};

#endif //_ADMINTHREAD_H__
