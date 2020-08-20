// AdminSession.cpp: implementation of the CAdminSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oats.h"
#include "AdminSession.h"
#include "OatsCallback.h"
#include "Logger.h"
#include "Message.h"
#include "SecTypeFilter.h"
#include "OatsDlg.h"
#include "RequestFactory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern COatsApp theApp;
extern CAdminThread* pTheThread;

CAdminSession::CAdminSession( COatsCallback * pDoc ) : CSessionBase(pDoc,'\n')
{
	m_bConnected = true;
	m_LoggedOn = false;
	m_NewClient = false;
	m_Mode = CSecTypeFilter::ALL;
	m_OwnerFilter.excluded = false;
}

CAdminSession::~CAdminSession()
{

}

void CAdminSession::OnMessage( CString & szMsg )
{
	if( !m_bConnected )
		return;

	CCsv* pMsg = new CCsv(szMsg);
	
	if (pMsg->cGet(CSV_COMMAND) == (char)(CMessage::Logon))
	{
		m_Mode = pMsg->nGet(CSV_INSTRUCTIONS);
		m_LoggedOn = true;

		if ((pMsg->Get(CSV_TRADER))[0] != '_')
		{
			m_NewClient = true;
		}

		CString excludedOwners = pMsg->Get(CSV_EXCLUDED_OWNERS);
		if (excludedOwners.IsEmpty() == 0)
		{
			SetExecludedOwners(excludedOwners);
		}

		m_pDoc->RomInstanceUpdate(ID());
	}
	else 
	{
		pMsg->SetAt(CSV_CLIENT_FLEAK, ID());
		theApp.PostThreadMessage(TF_ADMIN_REQUEST, 0, (long)pMsg);
	}
}

void CAdminSession::OnClose( int nErrorCode )
{
	m_bConnected = false;
	if (pTheThread)
	{
		pTheThread->PostThreadMessage(TF_ADMIN_DISCONNECT, 0, (LPARAM)this);
	}
}


int CAdminSession::Send( const CString & szMsg )
{
	int rc = -1;

	if (false == m_LoggedOn)
	{
		return rc;
	}

	CString lMsg = szMsg;
	CCsv msg(lMsg);

	if (msg.cGet(CSV_COMMAND) == (char)(CMessage::ResponseToAdmin))
	{
		if (true == m_NewClient)
		{
			rc = CSessionBase::Send(szMsg );
		}
	}
	else
	{
		char SecType = msg.cGet(CSV_SECURITY_TYPE);
		CString owner = msg.Get(CSV_OWNER);

		if (CSecTypeFilter::Block(SecType, m_Mode) == false &&
			OwnerIncluded(owner))
		{
			rc = CSessionBase::Send(szMsg );
		}
	}

	return rc;
}


bool CAdminSession::OwnerIncluded(const CString& owner)
{
	bool rc = true;

	tStrSetIterator it = (m_OwnerFilter.OwnerList).find(owner);
	if (it != (m_OwnerFilter.OwnerList).end())
	{
		if (true == m_OwnerFilter.excluded)
		{
			rc = false;
		}
	}

	return rc;
}

void CAdminSession::SetExecludedOwners(CString& owners)
{
	if (owners[0] == '-')
	{
		m_OwnerFilter.excluded = true;
	}
	else
	{
		m_OwnerFilter.excluded = false;
	}

	owners = owners.Mid(1);
	int index = 0;
	CString owner;

	while ((index = owners.Find("|")) != -1)
	{
		owner = owners.Left(index);
		(m_OwnerFilter.OwnerList).insert(owner);
		owners = owners.Mid(index + 1);
	}

	if (owners.IsEmpty() == 0)
	{
		(m_OwnerFilter.OwnerList).insert(owners);
	}

}

