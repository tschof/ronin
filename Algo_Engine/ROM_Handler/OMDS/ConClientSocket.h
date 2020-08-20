// ConClientSocket.h: interface for the CConClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONCLIENTSOCKET_H__4B1E7719_36BF_43A8_B70D_F80EC33805A9__INCLUDED_)
#define AFX_CONCLIENTSOCKET_H__4B1E7719_36BF_43A8_B70D_F80EC33805A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
class CConClient;

class CConClientSocket : public CAsyncSocket
{
public:
	friend class CConClient;
	void			onShutdown();
	void			setMaxQueueLength(int maxQueueLength);
	void			sendData( const std::string & data);
	void			pendingSend( const std::string & data);
	int				socketSend( const std::string & data );
	void			receiveForReadLine();
	std::string		readLine();
	int				getClientId();

	void			AddRef()
	{
		InterlockedIncrement( &m_nCount );
	}

	void			Release()
	{
		InterlockedDecrement( &m_nCount );
		if( m_nCount == 0 )
			delete this;
	}
	CConClientSocket(CConClient * pConClient);
	virtual ~CConClientSocket();

	int				getStatus() { return m_errorCode; }

	void			shutDown(){ onShutdown(); }

	virtual void	OnClose( int nErrorCode );
	virtual void	OnReceive( int nErrorCode );
	virtual void	OnSend( int nErrorCode );

	void			SetLogin( bool bLogin ){ m_bLogin = bLogin; }
	bool			GetLogin() { return m_bLogin; }

	void			SetUser( const std::string & szUser ){ m_szUser = szUser; }
	std::string		GetUser() { return m_szUser; }

	bool			SetLinger();

private:
	long			m_nCount;
	bool			m_bLogin;
	std::string		m_szUser;
	int				m_maxQueueLength;
	long			m_clientId;
	char			m_cBuffer[2001];
	DWORD			m_errorCode;
	CLog			m_log;
	CConClient *	m_pCConClient;
	StringList		m_newDataList;
	std::string		m_sBuffer;
	Lock			m_Lock;
};

#endif // !defined(AFX_CONCLIENTSOCKET_H__4B1E7719_36BF_43A8_B70D_F80EC33805A9__INCLUDED_)
