// HostConnection.h: interface for the CHostConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOSTCONNECTION_H__F3AADB11_DBB9_11D1_8CFA_006008CD4894__INCLUDED_)
#define AFX_HOSTCONNECTION_H__F3AADB11_DBB9_11D1_8CFA_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Connection.h"

class CHostConnection : public CConnection  
{
private:
	CTime			m_LastReception;
	CTime			m_LastTransmission;
	BOOL			m_Reconnect;

public:
				CHostConnection(const CString& Name, const CString& AppName, const CString& Path);
	virtual	~CHostConnection();

public:
	const CTime&	GetLastReception()		const { return m_LastReception; }
	const CTime&	GetLastTransmission()	const { return m_LastTransmission; }
			BOOL		SendMessage(CString);
	const BOOL		Reconnect()					const { return m_Reconnect; }
			void		SetReconnect(BOOL Reconnect)	{ m_Reconnect = Reconnect; }

	void	ProcessMessage(CString&);
	void	ProcessError(const CString& Error);
	void	ProcessLogEntry(const CString& Entry);
	void  ProcessStatus(const CString& Entry);
	

private:
	void		OnConnect(int);
	void		OnClose(int nErrorCode);

};



#endif // !defined(AFX_HOSTCONNECTION_H__F3AADB11_DBB9_11D1_8CFA_006008CD4894__INCLUDED_)
