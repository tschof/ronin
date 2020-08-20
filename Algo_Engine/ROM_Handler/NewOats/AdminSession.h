#ifndef _ADMINSESSION_H__
#define _ADMINSESSION_H__

#include <set>
#include <queue>
#include "Socket.h"

#pragma once

using namespace RASL;

class CAdminSession : public CRSocket
{
public:
	CAdminSession(SOCKET sock);
	~CAdminSession(void);
	short ID() { return m_ID; }
	void SetID(int value) { m_ID = value; }
	bool OwnerIncluded(const std::string& owner);
	void SetExecludedOwners(std::string& owners);
	int Mode()	{ return m_Mode; }
	void SetMode(int value) { m_Mode = value; }
	void SetLoggedOn(bool value) { m_LoggedOn = value; }
	bool LoggedOn() { return m_LoggedOn; }
	void SetIncludedDests(std::string& dests);
	bool DestIncluded(short dest);

	void OnSend(int error);
	void Send(std::string* pMsg);
	void Peek(std::string* pMsg);
	void GetPeerIP(std::string& IP);

private:
	
	short m_ID;
	bool m_LoggedOn;
	int m_Mode;
	SOCKET m_Socket;

	std::set<short> m_includedDests;
	std::set<short> m_excludedDests;

	std::set<std::string> m_includedOwners;
	std::set<std::string> m_excludedOwners;
	std::deque<std::string*> m_msgQ;
	//std::queue<std::string*> m_msgQ;

	bool QueueMsg(std::string* pMsg);

};

#endif //_ADMINSESSION_H__
