// OmSession.cpp : implementation file
//

#include "stdafx.h"
#include "MessageCracker.h"
#include "OmSession.h"
#include "IOmApplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COmSession

COmSession::COmSession( IOMApplication * pApp ) : m_pApp( pApp )
{
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

void COmSession::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	char szBuff[4096];
	int nSize = -1;
	ZeroMemory( szBuff, sizeof(szBuff) );
	nSize = CAsyncSocket::Receive( szBuff, sizeof(szBuff) );
	while( nSize > 0 )
	{
		int nIndex = -1;
		CString szMsg = m_szLeft + szBuff;
		nIndex = szMsg.Find( '\n' );
		while( nIndex != -1 )
		{
			CString msg = szMsg.Left( nIndex );
			try
			{
				m_pApp->OnMessage( std::string((LPCSTR)msg) );
			}
			catch(...)
			{
			}
			szMsg = szMsg.Mid( nIndex+1 );
			nIndex = szMsg.Find( '\n' );
		}
		m_szLeft = szMsg;
		ZeroMemory( szBuff, sizeof(szBuff) );
		nSize = CAsyncSocket::Receive( szBuff, sizeof(szBuff) );
	}
	CAsyncSocket::OnReceive(nErrorCode);
}
