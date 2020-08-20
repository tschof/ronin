#if !defined(AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_)
#define AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActorConnection.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CActorConnection command target
#include "ActHost.h"

class CActorConnection : public CConnection//public CAsyncSocket
{
// Attributes
public:
	static CString m_sLastHost;
	static void GetLastHost();
	static void SetLastHost(CString &Host);

// Operations
public:
	CActorConnection(const CString& Name, const CString& AppName, const CString& Path);
	virtual ~CActorConnection();

	BOOL SendMessage(char *Message, int Size);

	//void ProcessTimer();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActorConnection)
	//}}AFX_VIRTUAL
	virtual	void OnClose(int nErrorCode);
	virtual	void OnConnect(int nErrorCode);
	virtual	void OnReceive(int nErrorCode);
	virtual void ConnectToHost(int Port = 0);

	//virtual void OnSend(int nErrorCode);
	//virtual	void PostError(int Error, BOOL bClose = TRUE);
	// Generated message map functions
	//{{AFX_MSG(CActorConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
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

	CActHost		m_ActHost;
	int				RMsgSize() { return m_RMsgSize; }
	int				SMsgSize() { return m_SMsgSize; }

	void ClearRMsg() 
	{ 
		memset(m_RMsgBuffer, 0, 4096);
		m_RMsgSize = 0;
		m_RLeftOverBytes = 0;
		m_pRPos = m_RMsgBuffer;
	}

	void ClearSMsg() 
	{ 
		memset(m_SMsgBuffer, 0, 4096);
		m_SMsgSize = 0;
		m_SLeftOverBytes = 0;
		m_pSPos = m_SMsgBuffer;
	}

	//virtual	void ReadParameters(const CString& AppName, const CString& Path);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTORCONNECTION_H__CF23D013_B685_11D5_A622_00500401F87A__INCLUDED_)
