// OmListener.cpp : implementation file
//

#include "stdafx.h"
#include "MessageCracker.h"
#include "OmListener.h"
#include "IOmApplication.h"
#include "OmSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COmListener

COmListener::COmListener( IOMApplication * pApp ) : m_pApp( pApp )
{
}

COmListener::~COmListener()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(COmListener, CAsyncSocket)
	//{{AFX_MSG_MAP(COmListener)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// COmListener member functions

void COmListener::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	COmSession * pSession = new COmSession( m_pApp );
	Accept( *pSession );
	m_pApp->SetSession( pSession );
	CAsyncSocket::OnAccept(nErrorCode);
}
