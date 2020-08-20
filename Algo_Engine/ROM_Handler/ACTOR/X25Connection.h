// X25Connection.h: interface for the CX25Connection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X25CONNECTION_H__F89DBDA1_9699_11D2_9F3F_00105AA84069__INCLUDED_)
#define AFX_X25CONNECTION_H__F89DBDA1_9699_11D2_9F3F_00105AA84069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "X25Socket.H"
#include "ConnectionHeader.h"

class CX25Connection : public CX25Socket, public CWinThread
{
public:
	CX25Connection(const CString& Name, const CString& AppName, const CString& Path, unsigned char Card = 0, WORD SendChannel = 0, WORD ReceiveChannel = 0);
	virtual ~CX25Connection();
public:
	CString		m_Name;
	BOOL		m_Connected;
	CString		m_ConnectionTime;
	long		m_Sent;
	long		m_Received;

private:
	BOOL		m_LoggedOn;
	CString		m_LeftOver;

public:
	void SetLeftOver(CString& LeftOver)		{ m_LeftOver = LeftOver; }
	BOOL Connected()			{ return m_Connected;		}
	BOOL LoggedOn()				{ return m_LoggedOn;		}
	CString ConnectionTime()	{ return m_ConnectionTime;	}
	void ConnectToHost(BOOL bReport= TRUE);
	void SetLogonStatus(BOOL, BOOL bReport = TRUE);
	BOOL SendMessage(CString);
	//void Close();

private:
	CString	GetErrorText(int nCommand, int nErrorNum);

protected:
	virtual void ProcessMessage(CString&) = 0;
	virtual void ProcessError(const CString& Error) = 0;
	virtual void ProcessLogEntry(const CString& Entry) = 0;
	virtual void ProcessStatus(const CString& Entry) = 0;
	virtual	void PostError(int Command, int Error);

	//virtual void PostError(int Error);
};

#endif // !defined(AFX_X25CONNECTION_H__F89DBDA1_9699_11D2_9F3F_00105AA84069__INCLUDED_)
