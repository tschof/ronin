// AdminServer.cpp : implementation file
//

#include "stdafx.h"
#include "oats.h"
#include "AdminServer.h"
#include "Logger.h"
#include "AdminSession.h"
#include "OatsDlg.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdminServer
extern COatsApp theApp;

CAdminServer::CAdminServer()
{
}

CAdminServer::~CAdminServer()
{
	tAdmins::iterator it;
	for( it = m_Admins.begin(); it != m_Admins.end(); it++ )
		delete (*it);
	m_Admins.clear();
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CAdminServer, CAsyncSocket)
	//{{AFX_MSG_MAP(CAdminServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CAdminServer member functions

void CAdminServer::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	Logger::getInstance().LogError( "Order Master Client Connected" );
	CAdminSession * pSession = new CAdminSession( COatsDlg::GetCallback() );
	Accept( *pSession );
	m_Admins.push_back( pSession );
	CAsyncSocket::OnAccept(nErrorCode);
	theApp.PostThreadMessage(TF_ADMINCOUNT_UPDATE, 0, Count());
}

void CAdminServer::Send( CString szMsg )
{
	tAdmins::iterator it;
	for( it = m_Admins.begin(); it != m_Admins.end(); it++ )
		(*it)->Send( szMsg );
}

void CAdminServer::Send(CCsv& szMsg)
{
	if (szMsg.cGet(CSV_COMMAND) != (char)(CMessage::ResponseToAdmin))
	{
		(COatsDlg::GetCallback())->WriteCSV(&szMsg);
		Send(szMsg.Build());
		return;
	}

	tAdmins::iterator it;
	for( it = m_Admins.begin(); it != m_Admins.end(); it++ )
	{
		if ((szMsg.Get(CSV_CLIENT_FLEAK)).IsEmpty() == FALSE)
		{
			if( (*it)->ID() == szMsg.nGet(CSV_CLIENT_FLEAK) )
			{
				(*it)->Send( szMsg.Message());
				break;
			}
		}
		else
		{
			(*it)->Send( szMsg.Message());
		}
	}
}

void CAdminServer::Remove( CAdminSession * pSession )
{
	tAdmins::iterator it;
	for( it = m_Admins.begin(); it != m_Admins.end(); it++ )
	{
		if( (*it) == pSession )
		{
			delete pSession;
			m_Admins.erase( it );
			return;
		}
	}
}
