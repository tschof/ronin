// X25HostConnection.h: interface for the CX25HostConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X25HOSTCONNECTION_H__BCF48875_98E5_11D2_9F45_00105AA84069__INCLUDED_)
#define AFX_X25HOSTCONNECTION_H__BCF48875_98E5_11D2_9F45_00105AA84069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#define X25_STARTLISTENING WM_USER + 20

#include "X25Connection.h"

class CX25HostConnection : public CX25Connection  
{
private:
	CTime	m_LastReception;
	CTime	m_LastTransmission;
	BOOL	m_Reconnect;

public:
	CX25HostConnection(const CString& Name, const CString& AppName, const CString& Path, unsigned char Card = 0, WORD SendChannel = 0, WORD ReceiveChannel = 0);
	virtual ~CX25HostConnection();

public:
	const CTime&	GetLastReception()		const { return m_LastReception;    }
	const CTime&	GetLastTransmission()	const { return m_LastTransmission; }
	      BOOL		SendMessage(CString);
	const BOOL		Reconnect()				const { return m_Reconnect;        }
		  void		SetReconnect(BOOL Reconnect)  { m_Reconnect = Reconnect;   }

	void ProcessMessage(CString&);
	void ProcessError(const CString& Error);
	void ProcessLogEntry(const CString& Entry);
	void ProcessStatus(const CString& Entry);

private:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	virtual BOOL OnIdle( LONG lCount );

	CObList	m_SendQueue;

	//void Listen(WPARAM wParam, LPARAM lParam);

	//DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_X25HOSTCONNECTION_H__BCF48875_98E5_11D2_9F45_00105AA84069__INCLUDED_)
