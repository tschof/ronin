// Clients.cpp: implementation of the CClients class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Clients.h"
#include "ClientConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CClient::CClient()
{
	m_Client			= "New Connection";
	m_pConnection	= new CClientConnection();
}

CClient::~CClient()
{
	delete m_pConnection;
}

CClientConnection *CClients::Create()
{
	CClient				*pClient;
	
	if (pClient = new CClient())
		{
		if (Add(pClient))
			{
			if (m_pList) m_pList->Add(pClient->Connection());
			return pClient->Connection();
			Update();
			}
		}

return NULL;
}

CClientConnection *CClients::FindClient(const char *Name)
{
	CClientConnection *pClient = NULL;

	if (m_ClientToConnection.Lookup(Name, (CObject *&)pClient))
		return pClient;
	else 
		return NULL;
}

POSITION CClients::Add(CClient *pClient)
{
	return AddTail(pClient);
}

void CClients::DeleteAll()
{
	while (!(IsEmpty()))
		delete (CClient *)RemoveHead();

	RemoveAll();
}

POSITION CClients::Delete(CClient *pClient)
{
	return Delete(pClient->Connection());
}

POSITION CClients::Delete(CClientConnection *pConnection)
{
	POSITION				pos;
	CClient				*pCurClient;

	for (pos = GetHeadPosition(); (pos); )
		{
		if (pCurClient = (CClient *)GetAt(pos))
			{
			if (pCurClient->Connection() == pConnection)
				{
				if (m_pList) m_pList->Delete(pConnection);
				m_ClientToConnection.RemoveKey(pCurClient->Client());
				RemoveAt(pos);
				delete pCurClient;
				return pos;
				}
			else
				GetNext(pos);
			}
		else
			{
			RemoveAt(pos);
			GetNext(pos);
			}
		}

return NULL;
}

void CClients::Update()
{
	if (m_pList)
		{
		m_pList->Invalidate(FALSE);
		m_pList->UpdateWindow();
		}
}

void CClients::SetClient(CClientConnection *pConnection, const CString& Client)
{
	POSITION				pos;
	CClient				*pCurClient;

	for (pos = GetHeadPosition(); (pos); )
		{
		if (pCurClient = (CClient *)GetAt(pos))
			{
			if (pCurClient->Connection() == pConnection)
				{
				pCurClient->SetClient(Client);
				m_ClientToConnection[Client] = pCurClient;
				Update();
				pos = NULL;
				}
			else
				GetNext(pos);
			}
		}
}

BOOL CClients::Send(const CString& Client, const CString& Message)
{
	CClient	*pClient = NULL;

	if (m_ClientToConnection.Lookup(Client, (CObject *&)pClient))
		return(pClient->Send(Message));
		
return(FALSE);

}

BOOL CClient::Send(const CString& Message)
{
return (m_pConnection->SendMessage(Message));	
}