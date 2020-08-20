// ConClientListener.cpp: implementation of the CConClientListener class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	CConClientListener
//	The client connection is composed of two main pieces
//	The listener (this object) and the CConClientSocket (the actua
//	client connections).  Each has its own threads for handling io.
//	both extend the Client Socket object CSocket.
//	The only thing this object needs to do is to accept client connection
//
#include "stdafx.h"
#include "ConClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConClientListener::CConClientListener(CConClient * pConClient)
{
	m_pCallback = pConClient;
}

CConClientListener::~CConClientListener()
{

}

void CConClientListener::pendingAccept()
{
	m_pCallback->acceptConnection(this);
}

void CConClientListener::OnAccept( int nErrorCode )
{
	pendingAccept();
}