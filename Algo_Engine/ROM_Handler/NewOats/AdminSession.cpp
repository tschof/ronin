#include "stdafx.h"
#include "AdminSession.h"
#include "SecTypeFilter.h"
#include "Config.h"
#include "Logger.h"

CAdminSession::CAdminSession(SOCKET sock) : CRSocket(sock)
{
	//m_LoggedOn = false;
	m_Mode = CSecTypeFilter::ALL;
	m_LoggedOn = false;
	m_Socket = sock;
	SetUseDynamicBuffer(false);
}

CAdminSession::~CAdminSession(void)
{
	while (m_msgQ.empty() == false)
	{
		std::string* pMsg = NULL;
		pMsg = m_msgQ.front();
		m_msgQ.pop_front();
		delete pMsg;
	}
}

void CAdminSession::Peek(std::string* pMsg)
{
	int error = 0;
	m_msgQ.push_front(pMsg);
	OnSend(error);;
}

void CAdminSession::Send(std::string* pMsg)
{
	int error = 0;
	
	if (QueueMsg(pMsg) == true)
	{
		OnSend(error);;
	}
	else
	{
		delete pMsg;
	}
}

void CAdminSession::OnSend(int error)
{
	std::string* pMsg = NULL;

	while (m_msgQ.empty() == false)
	{
		pMsg = m_msgQ.front();
		if (CRSocket::Send(*pMsg) == false &&
			GetLastError() == RASL::RASL_NO_ERROR)
		{
			break;
		}
		m_msgQ.pop_front();
		delete pMsg;
	}
}

bool CAdminSession::QueueMsg(std::string* pMsg)
{
	//if the queue size is smaller than the max length queue it
	bool rc = false;
	if (m_msgQ.size() < (CConfig::getInstance()).MaxClientQueueSize())
	{
		m_msgQ.push_back(pMsg);
		rc = true;
	}
	else
	{
		//Log error and send email
		static int nError = 0;
		if( nError < 5 )
		{
			nError++;
			std::string IP;
			GetPeerIP(IP);
			std::string subject = "OrderMaster may be slow";
			std::stringstream stm;
			stm << "OrderMaster connected from "
				<< IP
				<< " may be slow<"
				<< m_msgQ.size()
				<< " messages queued >\r\n";
			Logger::getInstance().LogError( stm.str());
			(CConfig::getInstance()).SendEmail(subject, stm.str());
		}
	}

	return rc;
}

bool CAdminSession::OwnerIncluded(const std::string& owner)
{
	bool rc = true;

	if (m_includedOwners.empty() == false)
	{
		std::set<std::string>::iterator it = m_includedOwners.find(owner);
		if (it == m_includedOwners.end())
		{
			rc = false;
		}
	}
	else if (m_excludedOwners.empty() == false)
	{
		std::set<std::string>::iterator it = m_excludedOwners.find(owner);
		if (it != m_excludedOwners.end())
		{
			rc = false;
		}
	}

	return rc;
}

void CAdminSession::SetExecludedOwners(std::string& owners)
{
	if (owners.empty() == true)
	{
		return;
	}

	m_includedOwners.clear();
	m_excludedOwners.clear();

	std::set<std::string>* pOwners = &m_includedOwners;
	if (owners[0] == '-')
	{
		pOwners = &m_excludedOwners;
		owners = owners.substr(1);
	}

	int curOff = owners.find('|');
	std::string buf;

	while (curOff != std::string::npos)
	{
		buf = owners.substr(0, curOff);
		pOwners->insert(buf);
		owners = owners.substr(curOff + 1);
		curOff = owners.find('|');
	}

	if (owners.empty() == false)
	{
		pOwners->insert(owners);
	}
}

void CAdminSession::SetIncludedDests(std::string& dests)
{
	if (dests.empty() == true)
	{
		return;
	}

	m_includedDests.clear();
	m_excludedDests.clear();

	std::set<short>* pDests = &m_includedDests;
	if (dests[0] == '-')
	{
		pDests = &m_excludedDests;
		dests = dests.substr(1);
	}

	int curOff = dests.find('|');
	std::string buf;

	while (curOff != std::string::npos)
	{
		buf = dests.substr(0, curOff);
		pDests->insert(atoi(buf.c_str()));
		dests = dests.substr(curOff + 1);
		curOff = dests.find('|');
	}

	if (dests.empty() == false)
	{
		pDests->insert(atoi(dests.c_str()));
	}
}

bool CAdminSession::DestIncluded(short dest)
{
	bool rc = true;

	if (m_includedDests.empty() == false)
	{
		std::set<short>::iterator it = m_includedDests.find(dest);
		if (it == m_includedDests.end())
		{
			rc = false;
		}
	}
	else if (m_excludedDests.empty() == false)
	{
		std::set<short>::iterator it = m_excludedDests.find(dest);
		if (it != m_excludedDests.end())
		{
			rc = false;
		}
	}

	return rc;
}

void CAdminSession::GetPeerIP(std::string& IP)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	int rPeerPort;
	char* rPeerAddress = NULL;

	if (!getpeername(m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen))
	{
		IP = inet_ntoa(sockAddr.sin_addr);
	}
}