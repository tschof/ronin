#ifndef _ROMSESSION_H__
#define _ROMSESSION_H__

#include "Socket.h"

#pragma once

using namespace RASL;

class CRomSession : public CRSocket
{
public:
	CRomSession(SOCKET sock);
	~CRomSession(void);
	int Login() { return m_Login; }
	void SetLogin(int romInstance, unsigned long lastIncomingSeq);
	void SetLoggedOn(bool value) { m_LoggedOn = value; }
	bool LoggedOn() { return m_LoggedOn; }
	unsigned long LastIncomingSeq();
	void UpdateLastIncomingSeq(unsigned int value);
	unsigned long CRomSession::GetlastIncomingSeq(int romInstance);
private:
	int m_Login; //Rom instance ID
	bool m_LoggedOn;
	SOCKET m_Socket;
	
	unsigned long m_lastIncomingSeq;
};

#endif //_ROMSESSION_H__
