#if !defined(AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_)
#define AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActorConnection.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CActorConnection command target

class CActorConnection : public CConnection//public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CActorConnection(const CString& Name, const CString& AppName, const CString& Path);
	virtual ~CActorConnection();

	void ConnectToHost(int Port);
	BOOL SendMessage(char *Message, int Size);
	void Close()
	{ 
		if (m_hSocket != INVALID_SOCKET)
		{
			CAsyncSocket::AsyncSelect(0);
			CAsyncSocket::Close();
			OnClose(0);
		}
 	}

	const CString&	Host()		const	{ return m_Host; }
	const int		Port()		const	{ return m_Port; }

	void ProcessTimer();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActorConnection)
	//}}AFX_VIRTUAL
	virtual	void OnClose(int nErrorCode);
	virtual	void OnConnect(int nErrorCode);
	virtual	void OnReceive(int nErrorCode);
	//virtual void OnSend(int nErrorCode);
	virtual	void PostError(int Error, BOOL bClose = TRUE);
	// Generated message map functions
	//{{AFX_MSG(CActorConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CString			m_Host;
	int				m_Port;
	BOOL			m_Connected;
	int				m_SLeftOverBytes;
	char			m_SMsgBuffer[4096];
	short			m_SMsgSize;
	int				m_RLeftOverBytes;
	char			m_RMsgBuffer[4096];
	short			m_RMsgSize;
	//CTime			m_LastReception;
	CTime			m_LastSendTime;
	char *			m_pRPos;
	char *			m_pSPos;

	int				RMsgSize() { return m_RMsgSize; }
	int				SMsgSize() { return m_SMsgSize; }

	CString	GetErrorText(int nErrorNum);

	void ClearRMsg() 
	{ 
		memcpy(m_RMsgBuffer, 0, 4096);
		m_RMsgSize = 0;
		m_LeftOverBytes = 0;
		m_pRPos = m_RMsgBuffer;
	}

	void ClearSMsg() 
	{ 
		memcpy(m_SMsgBuffer, 0, 4096);
		m_MsgSize = 0;
		m_SLeftOverBytes = 0;
		m_pSPos = m_SMsgBuffer;
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_)
