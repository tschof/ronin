// SessionBase.cpp : implementation file
//

#include "stdafx.h"
#include "Oats.h"
#include "SessionBase.h"
#include "OatsCallback.h"
#include "Logger.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSessionBase

CSessionBase::CSessionBase(COatsCallback * pDoc, char cTerm ) : m_pDoc(pDoc), m_cTerm(cTerm), m_bConnected(false)
, m_LastSendTime(CTime::GetCurrentTime()), m_LastRcvTime(CTime::GetCurrentTime())
{
	static int id = 0;
	m_ID = id++;

	m_szLeft = "";
	m_ReadyToSend = false;
}

CSessionBase::~CSessionBase()
{
	CString* pMsg = NULL;

	while(m_SendQueue.empty() == false)
	{
		pMsg = m_SendQueue.front();
		if (pMsg)
		{
			delete pMsg;
		}
		m_SendQueue.pop_front();
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CSessionBase, CAsyncSocket)
	//{{AFX_MSG_MAP(CSessionBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CSessionBase member functions

int	CSessionBase::Send1(const CString& szMsg)
{
	int nSize = CAsyncSocket::Send( (LPCSTR)szMsg, szMsg.GetLength() );


	if( nSize == SOCKET_ERROR )
	{
		DWORD dwError = GetLastError();
		if( dwError == WSAEWOULDBLOCK )
		{
			Logger::getInstance().LogError("CSessionBase::Send(): Would Block\n");
			TRACE( "CSessionBase::Send(): Would Block\n" );
			m_ReadyToSend = false;
		}
		else
		{
			std::stringstream stm;
			stm << "CSessionBase::Send Socket Error: " << dwError;
			Logger::getInstance().LogError( stm.str() );
		}
	}
	else
	{
		CTime t = CTime::GetCurrentTime();
		TRACE( "Sending Time: %02d:%02d:%02d\n", t.GetHour(), t.GetMinute(), t.GetSecond() );
	}

	return nSize;
}


int CSessionBase::Send( const CString & szMsg)
{
	int rc = -1;

	if( szMsg.IsEmpty() )
	{
		return rc;
	}

	CString* pMsg = new CString(szMsg);
	
	m_SendQueue.push_back(pMsg);

	//m_SendQueue.AddTail( szMsg );
	
	if (true == m_ReadyToSend)
	{
		if (DumpSendQueue() == true)
		{
			rc = 1;
		}
	}

	return rc;
}

void CSessionBase::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	m_ReadyToSend = true;
	DumpSendQueue();
	CAsyncSocket::OnSend(nErrorCode);
}


void CSessionBase::OnReceive( int nErrorCode )
{
    if (m_hSocket != INVALID_SOCKET) {
        int bytesRead = 0;
        int nIndex = -1;
        char szBuff[RECV_BUFF_SIZE];
        ZeroMemory(szBuff, RECV_BUFF_SIZE);
        AsyncSelect(0);
        bytesRead = CAsyncSocket::Receive(szBuff, RECV_BUFF_SIZE -1);
        if(bytesRead == SOCKET_ERROR) {
            std::stringstream stm;
            stm << "CSessionBase::OnReceive(): SOCKET ERROR: [" << GetLastError() << "]"<<" On Message "<<m_szLeft;
            Logger::getInstance().LogError(stm.str());     
        } else if(bytesRead == 0) {
            std::stringstream stm;
            stm << "CSessionBase::OnReceive(): ZERO BYTES READ HOST CLOSED SOCKET.";
            Logger::getInstance().LogError(stm.str());   
        } else {
            m_szLeft += szBuff;
            nIndex = m_szLeft.ReverseFind( '\n' );
			if (nIndex != -1)
			{
				CString msg = m_szLeft.Left( nIndex + 1 );
				OnMessage( msg );
				m_szLeft = m_szLeft.Mid(nIndex + 1);
			}

           /* while( nIndex != -1 ) {
                CString msg = m_szLeft.Left( nIndex );
                try {
                    CTime t = CTime::GetCurrentTime();
                    TRACE( "Receiving Time: %02d:%02d:%02d\n", t.GetHour(), t.GetMinute(), t.GetSecond() );
                    OnMessage( msg );
                    m_szLeft = m_szLeft.Mid( nIndex+1 );
                    nIndex = m_szLeft.Find( '\n' );
                } catch(...) {
                    std::stringstream stm;
                    stm << "CSessionBase::OnReceive(): Failed to process message: [" << msg << "]";
                    Logger::getInstance().LogError(stm.str());
                }
            }*/
        }
        if (m_hSocket != INVALID_SOCKET) {
            AsyncSelect();
        }
    }
}

int	CSessionBase::Send( const void* lpBuf, int nBufLen, int nFlags )
{
	m_LastSendTime = CTime::GetCurrentTime();
	return CAsyncSocket::Send( lpBuf, nBufLen, nFlags );
}


void CSessionBase::SendHeartBeat()
{
	Logger::getInstance().LogDebug("DBG:::Sending HB MSG");
	TRACE("DBG:::Sending HB MSG");
	if( -1 == Send( "H\n" ) )
	{
		Logger::getInstance().LogError("Failed to send HeartBeat");
	}
}


bool CSessionBase::DumpSendQueue()
{
	bool rc = true;
	CString* pMsg = NULL;

	while(m_SendQueue.empty() == false)
	{
		pMsg = m_SendQueue.front();
		if (Send1(*pMsg) < 0)
		{
			rc = false;
			break;
		}
		delete pMsg;
		m_SendQueue.pop_front();
	}

	/*POSITION pos = m_SendQueue.GetHeadPosition();
	while( pos )
	{
		CString szMsg = m_SendQueue.GetNext( pos );
		if( Send1( szMsg) < 0 )
		{
			rc = false;
			break;
		}
		m_SendQueue.RemoveHead();
	}*/

	return rc;
}
