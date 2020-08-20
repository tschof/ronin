#include "stdafx.h"
#include "RomSession.h"
#include "Config.h"

CRomSession::CRomSession(SOCKET sock) : CRSocket(sock)
{
	m_LoggedOn = false;
	m_Socket = sock;
	m_lastIncomingSeq = 0;
}

CRomSession::~CRomSession(void)
{
}

unsigned long CRomSession::LastIncomingSeq()
{
	return m_lastIncomingSeq;
}

void CRomSession::UpdateLastIncomingSeq(unsigned int value)
{
	m_lastIncomingSeq += value;
}

void CRomSession::SetLogin(int romInstance, unsigned long lastIncomingSeq)
{
	m_Login = romInstance;

	if (!m_lastIncomingSeq) //in case rom logon multiple times without disconnect
	{
		m_lastIncomingSeq = lastIncomingSeq;
	}
}


