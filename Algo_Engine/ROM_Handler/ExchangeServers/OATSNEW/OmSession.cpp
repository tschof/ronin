// OmSession.cpp : implementation file
//

#include "stdafx.h"
#include "Oats.h"
#include "OmSession.h"
#include "OatsCallback.h"
#include "Csv.h"
#include "Logger.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COmSession

COmSession::COmSession(COatsCallback * pDoc) : CSessionBase(pDoc,'\n')
{
	m_bConnected = true;
}

COmSession::~COmSession()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(COmSession, CAsyncSocket)
	//{{AFX_MSG_MAP(COmSession)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// COmSession member functions

void COmSession::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_bConnected = false;
	m_pDoc->m_OMServer.Remove( this );
	m_pDoc->RomInstanceUpdate();
	CAsyncSocket::OnClose(nErrorCode);
	delete this;
}

void COmSession::OnMessage( CString & szMsg )
{
	if (szMsg[0] == CMessage::Logon)
	{
		CCsv msg(szMsg);
		SetID(msg.nGet(CSV_TRADER));
		m_pDoc->RomInstanceUpdate();
	}
	else
	{
//		Logger::getInstance().LogDebug( (LPCSTR)szMsg );
		// SendHeartBeat();		// WXU,  don't need it anymore
		m_pDoc->onData( szMsg );
	}
}

void COmSession::ProcessTimer()
{
}



