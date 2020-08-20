// SlaveThread.cpp : implementation file
//

#include "stdafx.h"
#include "AdminThread.h"
#include "Oats.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OatsConnectThread
CAdminThread* pTheThread = NULL;
extern COatsApp theApp;

IMPLEMENT_DYNCREATE(CAdminThread, SlaveThread)

CAdminThread::CAdminThread()
{
	m_bAutoDelete = TRUE;;
	pTheThread = this;
}

CAdminThread::~CAdminThread()
{
}


//BEGIN_MESSAGE_MAP(SlaveThread, CWinThread)
	//{{AFX_MSG_MAP(SlaveThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SlaveThread message handlers
/*
int CAdminThread::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	CString	szDatFile = "c:\\Ordermanager\\Oats.dat";
	int nPort = GetPrivateProfileInt("Admin Server Information", "Port", 4100, szDatFile );
	if( FALSE == m_AdminServer.Create(nPort) )
		return false;
	if( FALSE == m_AdminServer.Listen() )
		return false;

	MSG msg;

	while( ::GetMessage( &msg, NULL, 0, 0 ) != -1 )
	{
		if( msg.message == WM_QUIT )
		{
			break;
		}
		else if( msg.message == TF_MSG_AVAILABLE )
		{
			OnProcessMessages( msg.wParam, msg.lParam );
		}

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return 0;
}*/


void CAdminThread::PreRun()
{
	bool rc = true;

	CString	szDatFile = "c:\\Ordermanager\\Oats.dat";
	int nPort = GetPrivateProfileInt("Admin Server Information", "Port", 4100, szDatFile );

	if( FALSE == m_AdminServer.Create(nPort) )
	{
		//rc = false;
	}

	if( FALSE == m_AdminServer.Listen() )
	{
		//rc = false;
	}

	//return rc;
}


void CAdminThread::ProcessMessage(CString& msg)
{
	int location = -1;
	CString str = "";

	while ((location = msg.Find('\n')) >= 0)
	{
		if (location == 0)
		{
			str = msg.Mid(1);
		}
		else
		{
			str = msg.Left(location);
			msg = msg.Mid(location+1);
		}
		
		CCsv csv(str);
		m_AdminServer.Send(csv);
	}
}


void CAdminThread::ProcessThreadInstruction(const MSG& msg)
{
	if (msg.message == TF_ADMIN_DISCONNECT)
	{
		CAdminSession* pSession = (CAdminSession*)msg.lParam;
		if (pSession)
		{
			m_AdminServer.Remove(pSession);
			theApp.PostThreadMessage(TF_ADMINCOUNT_UPDATE, 0, m_AdminServer.Count());
		}
	}
}



void CAdminThread::ProcessMessages(StringList& msgs)
{
	while (msgs.empty() == false)
	{
		CString msg = msgs.front();
		ProcessMessage(msg);
		msgs.pop_front();
	}
}