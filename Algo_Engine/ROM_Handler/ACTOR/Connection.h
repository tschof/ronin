#if !defined(AFX_CONNECTION_H__F3AADB10_DBB9_11D1_8CFA_006008CD4894__INCLUDED_)
#define AFX_CONNECTION_H__F3AADB10_DBB9_11D1_8CFA_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Connection.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnection command target

#include "ConnectionHeader.h"


class CConnection : public CAsyncSocket
{
// Attributes
public:
	CString			m_Name;
	BOOL				m_Connected;
	CString			m_ConnectionTime;
	long				m_Sent;
	long				m_Received;

private:
	CString			m_Host;
	int				m_Port;
	CString			m_Login;
	CString			m_LeftOver;
	BOOL				m_LoggedOn;

public:
	const CString& Name()	const { return m_Name; }

// Operations
public:
	void		SetLeftOver(CString& LeftOver)		{ m_LeftOver = LeftOver;	}
	BOOL		Connected()									{ return m_Connected;		}
	BOOL		LoggedOn()									{ return m_LoggedOn;			}
	CString	ConnectionTime()							{ return m_ConnectionTime; }
	void		ConnectToHost();
	void		SetLogonStatus(BOOL);
	virtual	BOOL	SendMessage(CString);	

	// constructor and destructor	
					CConnection(const CString& Name, const CString& AppName, const CString& Path);
					CConnection();
		virtual	~CConnection();

private:
	CString	GetErrorText(int nErrorNum);

// overridables
protected:
	virtual	void	ProcessMessage(CString&)					= 0;
	virtual	void	ProcessError(const CString& Error)		= 0;
	virtual	void	ProcessLogEntry(const CString& Entry)	= 0;
	virtual	void  ProcessStatus(const CString& Entry)		= 0;
	virtual	void	PostError(int Error);
	virtual	void  ReadParameters(const CString& AppName, const CString& Path);

	
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnection)
	//}}AFX_VIRTUAL
	public:
		virtual	void	OnClose(int nErrorCode);
		virtual	void	OnConnect(int nErrorCode);
		virtual	void	OnReceive(int nErrorCode);
	// Generated message map functions
	//{{AFX_MSG(CConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTION_H__F3AADB10_DBB9_11D1_8CFA_006008CD4894__INCLUDED_)
