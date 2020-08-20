#ifndef _OATSSERVER_H__
#define _OATSSERVER_H__

#include "Windows.h"
#include <map>
#include <list>

#include "RomSession.h"
#include "AdminThread.h"
#include "DBThread.h"
#include "ServerSocket.h"
#include "RomCsv.h"


#pragma once

static const DWORD SERVER_EVT_INDEX = 0;
static const DWORD ADMIN_EVT_INDEX = 1;
static const DWORD CHECK_ENDTIME_INDEX = 2;
static const DWORD MIN_ROM_DISCONNECT_EVT_INDEX = 3;
static const DWORD MAX_ROM_DISCONNECT_EVT_INDEX = 60;

static const char MESSAGE_END = '\n';

class COatsServer
{
public:
	typedef std::map<int, CRomSession*> tRomSessionMap;
	typedef tRomSessionMap::iterator tRomSessionMapIT;
	typedef std::list<int> tEventIndexList;
	typedef tEventIndexList::iterator tEventIndexListIT;
	typedef std::list<CRomSession*> tRomSessionList;
	typedef tRomSessionList::iterator tRomSessionListIT;

	COatsServer(void);
	~COatsServer(void);
	void Run();
	void SetLive(bool value) { m_bLive = value; }

	int ProcessMessage(std::string& l_message, CRomSession* pC);
private:

	void ProcessAdminMessages();
	void ProcessAdminMessage(CRomCsv* pCsv);
	void ProcessQuery(CRomCsv* pCsv);
	void ProcessRecover(CRomCsv* pCsv);
	int GetRomIDFromRomTag(const std::string& romTag);
	void SendToRom(std::string& msg, int romID = 0);

	bool InitServer();
	CRomSession* RemoveRomSessionFromMaps(int index);
	void Shutdown();
	int GetDisconnectEvtIndex();
	void FreeDisconnectEvtIndex(int index);
	void UpdateRomInstances(int adminID = -1);
	void CloseRomSession(CRomSession* pC);
	void DeleteDeadRomSessions();
	void SetEndTimeCheckTimer();
	bool SessionEnd();
	void CheckQueueRatio();
	void CheckQueueSize();

	unsigned int GetOatsMsgCnt(std::string& msgs);
	unsigned long GetLastIncmingSeq(int romInstance);
	unsigned long GetLastIncomingSeqFromFile(const std::string& filename);

	WSAEVENT m_wsaEvents[MAX_ROM_DISCONNECT_EVT_INDEX + 1];
	tEventIndexList m_EvtIndexList;
	tRomSessionMap m_RomLoginToSessionMap;
	tRomSessionMap m_RomDisconnectEvtIndexToSessionMap;
	CAdminThread* m_pAdminThread;
	CDBThread* m_pDBThread;
	RASL::ServerSocket* m_pServerSock;
	int m_adminCount;
	int m_romCount;
	//long m_rvdOatsMsgCount;
	tRomSessionList m_DeadRomSessionList;

	bool m_bLive;
};

#endif //_OATSSERVER_H__
