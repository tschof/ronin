// Clients.h: interface for the CClients class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTS_H__3F37D242_1CA8_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_CLIENTS_H__3F37D242_1CA8_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CClientConnection;
#include "ConnectionList.h"

class CClient : public CObject
{
private:
	CClientConnection *m_pConnection;
	CString				m_Client;

public:
	CClientConnection *Connection()			{ return m_pConnection; }
	const CString&		Client()					{ return m_Client;		}

	void	SetClient(const CString& Client)	{ m_Client = Client;		}
	BOOL  Send(const CString& Message);

public:
	CClient();
	virtual ~CClient();
};

class CClients : public CObList  
{
private:
	CConnectionList	*m_pList;
	POSITION				Add(CClient *pClient);
	CMapStringToOb		m_ClientToConnection;

public:
	void					SetClient(CClientConnection *pConnection, const CString& Client);
	POSITION				Delete(CClient *pClient);
	POSITION				Delete(CClientConnection *pConnection);
	void					DeleteAll();
	CClientConnection	*Create();
	void					SetList(CConnectionList *pList)		{ m_pList = pList; }
	CClientConnection *FindClient(const char *Name);
	void					Update();
	BOOL					Send(const CString& Client, const CString& Message);
		


public:
	CClients()				{ }
	virtual ~CClients()	
		{ 
		m_ClientToConnection.RemoveAll();
		DeleteAll();
		}

};

#endif // !defined(AFX_CLIENTS_H__3F37D242_1CA8_11D2_8D11_006008CD4894__INCLUDED_)
