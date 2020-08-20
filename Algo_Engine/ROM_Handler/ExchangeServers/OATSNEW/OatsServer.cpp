// OatsServer.cpp : implementation file
//

#include "stdafx.h"
#include "oats.h"
#include "OatsServer.h"
#include "OmSession.h"
#include "OatsDlg.h"
#include "Logger.h"
#include <sstream>
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COatsServer

COatsServer::COatsServer() 
{
}

COatsServer::~COatsServer()
{
	OMSessions::iterator it;
	for (it = m_pSessions.begin(); it != m_pSessions.end(); it++)
	{
		delete *it;
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(COatsServer, CAsyncSocket)
	//{{AFX_MSG_MAP(COatsServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// COatsServer member functions

void COatsServer::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	COmSession* pOMSession = new COmSession( COatsDlg::GetCallback() );
	Accept( *pOMSession );
	m_pSessions.push_back(pOMSession);
	CAsyncSocket::OnAccept(nErrorCode);
}
/*
bool COatsServer::IsConnected()
{
	if( !m_pSession )
		return false;
	return m_pSession->IsConnected();
}
*/
void COatsServer::HeartBeat()
{
	OMSessions::iterator it = m_pSessions.begin();

	while (it != m_pSessions.end())
	{
		if (-1 == (*it)->Send( "Oats Heart Beat\n" ) )
		{
			COmSession* pSession = *it;
			int dwError = GetLastError();
			std::stringstream stm;
			stm << "COatsServer::HeartBeat socket error: " << dwError;
			Logger::getInstance().LogError(stm.str());
			it = m_pSessions.erase(it);
			delete pSession;
		}
		else
		{
			it++;
		}
	}

/*
	OMSessions::iterator it;
	for (it = m_pSessions.begin(); it != m_pSessions.end(); it++)
	{
		if (-1 == (*it)->Send( "Oats Heart Beat\n" ) )
		{
			COmSession* pSession = *it;
			int dwError = GetLastError();
			std::stringstream stm;
			stm << "COatsServer::HeartBeat socket error: " << dwError;
			Logger::getInstance().LogError(stm.str());
			it = m_pSessions.erase(it);  //it is the next one already!!!
			delete pSession;
		}
	}*/
}

void COatsServer::Send(CCsv& szMsg)
{
	OMSessions::iterator it;

	for( it = m_pSessions.begin(); it != m_pSessions.end(); it++ )
	{
		if (szMsg.cGet(CSV_COMMAND) == (char)CMessage::Status ||
			szMsg.cGet(CSV_COMMAND) == (char)CMessage::Zap ||
			szMsg.cGet(CSV_COMMAND) == (char)CMessage::Cancel )
		{
			if ((*it)->ID() == GetOMInstanceIdFromOMTag(szMsg.Get(CSV_STAFFORD_TAG)))
			{
				(*it)->Send( szMsg.Message());
				break;
			}
		}
		else
		{
			if (szMsg.nGet(CSV_SERVER_FLEAK) == 0) // broadcasting
			{
				(*it)->Send( szMsg.Message());
			}
			else if( (*it)->ID() == szMsg.nGet(CSV_SERVER_FLEAK) ) // to a specific OM
			{
				(*it)->Send( szMsg.Message());
				break;
			}
		}
	}
}


void COatsServer::Remove( COmSession * pSession )
{
	OMSessions::iterator it;

	for( it = m_pSessions.begin(); it != m_pSessions.end(); it++ )
	{
		if( (*it) == pSession )
		{
			m_pSessions.erase( it );
			return;
		}
	}
}


int COatsServer::OMCount()
{
	return m_pSessions.size();
}


int COatsServer::GetOMInstanceIdFromOMTag(CString omTag)
{
	int rc = 2;
	int k = omTag.ReverseFind('-');

	if (!(k < 9)) //not a invalid omTag
	{
		char s[2] = {'\0', '\0'};
		s[0] = omTag[k - 8];
		rc = atoi(s);
	}

	return rc;
}


void COatsServer::GetRomInstances(CString& roms)
{
	int id;
	OMSessions::iterator it = m_pSessions.begin();
	CString sID;
	roms = "";

	while (it != m_pSessions.end())
	{
		id = (*it)->ID();

		if (it == m_pSessions.begin())
		{
			roms.Format("%d", id);
		}
		else
		{
			sID.Format("|%d", id);
			roms += sID;
		}

		it++;
	}
}
