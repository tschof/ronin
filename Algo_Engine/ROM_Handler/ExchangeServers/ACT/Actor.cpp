// TerraNova.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Actor.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
//#include "X25Socket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaApp

BEGIN_MESSAGE_MAP(CTerraNovaApp, CWinApp)
	//{{AFX_MSG_MAP(CTerraNovaApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_LOGOUT, OnLogout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaApp construction

CTerraNovaApp::CTerraNovaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTerraNovaApp object

CTerraNovaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaApp initialization

BOOL CTerraNovaApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	CActorConnection::GetLastHost();

/*
	if (!XAPISetup())
	{
		AfxMessageBox(IDP_XAPI_INIT_FAILED);
		return FALSE;
	}

	if(u_u_short Cards = HIWORD(XAPIGetCards()))
	{
		for(int i = 1, j = 0; i<=pow(2,16); i*=2, j++)
		{
			if(Cards & i)
			{
				m_Card = j;
				break;
			}
		}
		if(!XAPIRegisterCard(m_Card))
		{
			AfxMessageBox(IDP_XAPI_CARD_FAILED);
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox(IDP_XAPI_CARD_FAILED);
		return FALSE;
	}
*/
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Actor"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_TERRATYPE,
		RUNTIME_CLASS(CTerraNovaDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CTerraNovaView));
	AddDocTemplate(pDocTemplate);

	/*CMultiDocTemplate* pDocTemplate2;
	pDocTemplate2 = new CMultiDocTemplate(
		IDR_TERRATYPE,
		RUNTIME_CLASS(CTerraNovaDoc),
		RUNTIME_CLASS(CConnectionFrame),
		RUNTIME_CLASS(CTerraNovaView2));
	AddDocTemplate(pDocTemplate2);*/

	CMultiDocTemplate* pTrafficDocTemplate;
	pTrafficDocTemplate = new CMultiDocTemplate(
		IDR_TRAFFICTYPE,
		RUNTIME_CLASS(CTrafficDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CTrafficView));
	AddDocTemplate(pTrafficDocTemplate);


	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	//m_Path		= "\\FixClient\\";
	m_Name	= "ITG";
	m_DatFile = "\\OrderManager\\" + m_Name + ".DAT";
	if(!CMessageApp::InitInstance())
		return FALSE;
	m_Path		= "\\OrderManager\\";

	// create client window
		CConnectionFrame	*pNewFrame;
		CTerraNovaDoc		*pDoc;
		POSITION				pos;

		pDoc = (CTerraNovaDoc *)pDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
		pNewFrame->SetTitle("Client Connections");
		pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		pNewFrame->Restore();
		if ((pos = pDoc->GetFirstViewPosition()))
			{
  			m_pClientView = (CTerraNovaView *)pDoc->GetNextView(pos);
			m_Clients.SetList(&m_pClientView->m_List);
			}

	// create exchange window
	/*	pDoc = (CTerraNovaDoc *)pDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
		pNewFrame->SetTitle("Exchange Connection");
		pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		pNewFrame->Restore();
		if ((pos = pDoc->GetFirstViewPosition()))
			{
  			m_pExchangeView = (CTerraNovaView *)pDoc->GetNextView(pos);
			m_pExchangeView->CreateTimer(5000);
			}
	*/
	// create error, status, and traffic windows
		CTrafficDoc		*pTrafficDoc;

		pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
		pNewFrame->SetTitle("Order Traffic");
		pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
		pNewFrame->Restore();
		if ((pos = pTrafficDoc->GetFirstViewPosition()))
  			m_pTrafficView = (CTrafficView *)pTrafficDoc->GetNextView(pos);

		pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
		pNewFrame->SetTitle("Status Messages");
		pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
		pNewFrame->Restore();
		if ((pos = pTrafficDoc->GetFirstViewPosition()))
  			m_pStatusView = (CTrafficView *)pTrafficDoc->GetNextView(pos);


		pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
		pNewFrame->SetTitle("Error Messages");
		pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
		pNewFrame->Restore();
		if ((pos = pTrafficDoc->GetFirstViewPosition()))
  			m_pErrorView = (CTrafficView *)pTrafficDoc->GetNextView(pos);

		pDoc = (CTerraNovaDoc *)pDocTemplate->CreateNewDocument();
		pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
		pNewFrame->SetTitle("Exchange Connection");
		pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		pNewFrame->Restore();
		if ((pos = pDoc->GetFirstViewPosition()))
			{
  			m_pExchangeView = (CTerraNovaView *)pDoc->GetNextView(pos);
			//m_pExchangeView->CreateTimer(5000);
			}

		//OpenLogFile();
	
		//m_pDb			= new CCommonDatabase(m_Path + m_Name + ".DAT");	// create database
		//m_pDb->Connect();
		m_pServer	= new CServer();												// create server
		//m_pRv			= new CRvSession();											// create rv session
		m_pExchange	= new CActSession();											// create fix session
		//m_pHandler	= new CTerraHandler();										// create order handler
		//m_pStatus	= new CTerraStatus();										// create status handler
		m_pHost		= NULL;
		AddHost((CConnection*) CreateHost());
		m_pServer->StartListening();				// start listening for client connections

		// init static vars
			COrder::Init();
			//CAct::Init();

		m_pExchangeView->CreateTimer(5000);

	// The main window has been initialized, so show and update it.
		pMainFrame->ShowWindow(SW_SHOWNORMAL);
		pMainFrame->Restore();
		pMainFrame->UpdateWindow();

		//m_ActDialog = new CActDialog(pMainFrame);
		//m_ActDialog->Create(IDD_ACTDIALOG, pMainFrame);
		//m_ActDialog->ShowWindow(SW_SHOW);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTerraNovaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaApp commands

void	CTerraNovaApp::AddHost(CBaseConnection *pHost)
{
	//if (m_pHost)
	//	DeleteHost();

	//m_pHost	= new CX25HostConnection(m_Name, m_Name, m_Path, m_Card, m_SendChannel, m_ReceiveChannel);
	//m_pHost	= new CX25HostConnection(m_Name, m_Name, m_Path, 0, 1, 2);
	//m_pHost->CreateThread();
	//m_pExchangeView->m_List.InsertString(0, "");
	//m_pExchangeView->m_List.SetItemDataPtr(0, m_pHost);
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);

}

void	CTerraNovaApp::DeleteHost(CConnection *pHost)
{
	/*if (m_pHost)
		{
		//if (m_pHost->m_hSocket != INVALID_SOCKET)
			{
			//m_pHost->AsyncSelect(0);
			delete m_pHost;
			}

		m_pExchangeView->m_List.DeleteString(0);
		delete m_pHost;
		m_pHost = NULL;
		}*/
	if (pHost)
	{
		if (pHost->m_hSocket != INVALID_SOCKET)
		{
			pHost->AsyncSelect(0);
			pHost->Close();
		}

		m_pExchangeView->m_List.Delete(pHost);
		try
		{
			delete pHost;
		}
		catch(...)
		{
			ProcessError("COrderManagerApp::DeleteHost", "Exception!");
		}
		pHost = NULL;
	}
}

void	CTerraNovaApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}
	
void	CTerraNovaApp::ProcessStatus(const CString& Name, const CString& Message)
{
	ProcessLogEntry(Name, Message);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void	CTerraNovaApp::ProcessError(const CString& Name, const CString& Error)
{
	ProcessLogEntry(Name, Error);
	if (m_pErrorView) m_pErrorView->Add(Name + '\t' + Error);
}

/*void	CTerraNovaApp::ProcessTraffic(const CString& Name, const CString& Traffic)
{
	ProcessLogEntry(Name, Traffic);

	if (m_pTrafficView) m_pTrafficView->Add(Name + '\t' + Traffic);
}*/

void	CTerraNovaApp::ProcessLogEntry(const CString& Name, const CString& Entry, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Entry, Append);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Entry);
	/*CString tracestream;
	if (m_Log.m_pStream)
		{
		m_Log.WriteString(tracestream = CTime::GetCurrentTime().Format("%H%M%S") + "," + Name + "," + Entry + "," + '\n');
		TRACE(tracestream);
		m_Log.Flush();
		}
	*/
}
void	CTerraNovaApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CTerraNovaApp::ExitInstance() 
{
	//m_ActDialog->DestroyWindow();
	//delete m_ActDialog;
	
	//XAPIDeregisterCard(m_Card);
	//XAPICleanup();
	
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;

	return CMessageApp::ExitInstance();
//	if(m_pHost)
//		m_pHost->Close();

	//delete m_pServer;					// accepts client connections
	//delete m_pRv;
	//delete (CTerraHandler *)m_pHandler;
	//delete m_pExchange;

	
	//m_pHost->PostThreadMessage(WM_QUIT, NULL, NULL);
	//while(::WaitForSingleObject(m_pHost->m_hThread, 100) != WAIT_OBJECT_0);
	//delete m_pStatus;
	//delete m_pDb;
	
	//return CWinApp::ExitInstance();
}

void CTerraNovaApp::OnLogout() 
{
	//Exchange().Logout();
}

/*void CTerraNovaApp::OpenLogFile()
{
	CFileException fe;
	if (!(m_Log.Open(m_Path + m_Name + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, &fe)))
	{
		if (!(m_Log.Open(m_Path + m_Name + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, &fe)))
		{
		}
	}
		
	m_Log.SeekToEnd();
}*/


BOOL CTerraNovaApp::ProcessClientLogon(CBaseConnection& pC, CCsv& CSV)
{
	CLogin	Login;		
	CLogout	Logout;

	CSV.Build();

	if (!pC.LoggedOn())	// dup login sent, probably passing in the night
	{
		// grab traders acronym
		pC.m_Name = CSV.Get(CSV_TRADER);

		if (pC.m_Name.GetLength() > 15)
		{
			ProcessError("Logon", "Logon Name Too Big - " + pC.m_Name);
			pC.m_Name = pC.m_Name.Left(15);
			ProcessError("Logon", "Truncated Name - " + pC.m_Name);
		}

		// associate trader with client connection
		if (Clients().SetClient(&pC, pC.m_Name)) // will return false if there is someone already logged in with the same name
		{
			pC.SetLogonStatus(TRUE);

			Clients().Update();
								
			// send login reply back to client
			Login.SetTrader(pC.m_Name);
			pC.SendMessage(Login.Message());

			// send client any stored messages
			Clients().Dump(pC.m_Name);

			// send client messages in shared window
			if (!Window().IsConnected())
				Window().Reconnect();

			Window().Dump(Name(), pC.m_Name);
		}
		else
		{
			// send logout command
			Logout.SetTrader(pC.m_Name);
			pC.SendMessage(Logout.Message());
			pC.Close();
		}
	}

	return TRUE;
}

BOOL CTerraNovaApp::ProcessClientLogout(CBaseConnection& pC, CCsv& CSV)
{
	CLogout	Logout;

	CSV.Build();

	// send logout reply
	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	// close
	pC.Close();

	Clients().Update();

	return TRUE;
}

BOOL CTerraNovaApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CSV.SetAt(CSV_OWNER, pC.Name());
	((CActSession *)m_pExchange)->ProcessActEntry(&CSV);
	return TRUE;
}


void CTerraNovaApp::ProcessActorMessage(const char* Msg, int Size)
{
	((CActSession *)m_pExchange)->ProcessMessage(Msg, Size);
}
