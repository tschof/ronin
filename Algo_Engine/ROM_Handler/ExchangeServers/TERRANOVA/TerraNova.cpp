// TerraNova.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TerraNova.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "FixSession.h"
#include "SeqDlg.h"
#include "TerraNovaSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	void SetAboutData(CString& title, CString& version, CString& copyRight)
	{
		m_version	= version;
		m_copyRight = copyRight;
		m_title		= title;
	}

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_version;
	CString m_title;
	CString m_copyRight;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL OnInitDialog();

	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	
	//}}AFX_DATA_INIT

	 m_title		= "About ";
	 m_version		= "Unknown";
	 m_copyRight	= "Unknown";
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_title);

	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_VERSION);

	if (NULL != pEdit)
	{
		pEdit->SetWindowText(m_version);
	}

	pEdit = (CEdit*) GetDlgItem(IDC_COPYRIGHT);

	if (NULL != pEdit)
	{
		pEdit->SetWindowText(m_copyRight);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaApp

BEGIN_MESSAGE_MAP(CArcaApp, CWinApp)
	//{{AFX_MSG_MAP(CArcaApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_LOGOUT, OnLogout)
	ON_COMMAND(IDM_RESET_OUTGOING, OnResetOutgoing)
	ON_COMMAND(IDM_RESET_INCOMING, OnResetIncoming)
	ON_COMMAND(IDM_CLEAR_OUTGOING, OnClearOutgoing)
	ON_COMMAND(IDM_CLEAR_INCOMING, OnClearIncoming)
	ON_COMMAND(IDM_TAGGLE_DISPLAY, OnTaggleDisplay)
	ON_UPDATE_COMMAND_UI(IDM_TAGGLE_DISPLAY, OnUpdateTaggleDisplay)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaApp construction

CArcaApp::CArcaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDisplayTraffic = false;
}

CArcaApp::~CArcaApp()
{
	try
	{
		if(m_pHandler)
		{
			delete m_pHandler;
		}
	}
	catch(...)
	{
	}

	try
	{
		if(m_pServer)
		{
			delete m_pServer;
		}
	}
	catch(...)
	{
	}

	_CrtDumpMemoryLeaks();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CArcaApp object

CArcaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CArcaApp initialization

BOOL CArcaApp::InitInstance()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_TERRATYPE,
		RUNTIME_CLASS(CArcaDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CArcaView));
	AddDocTemplate(pDocTemplate);

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
	char pathbuf[256];
	GetModuleFileName(NULL, pathbuf, 256);
	char* p1 = strrchr(pathbuf, '.');
	char* p2 = strrchr(pathbuf, '_');
	*p1 = '\0';
	
	if (p2 != NULL)
	{
		m_Name.Format("TNTO_%s", p2 + 1);
	}
	else
	{
		m_Name = "TNTO";
	}
	
	m_DatFile = "c:\\OrderManager\\" + m_Name + ".DAT";

	//m_Name	= "TNTO";
	//m_DatFile = "\\OrderManager\\" + m_Name + ".DAT";
	if(!CMessageApp::InitInstance())
		return FALSE;
	m_Path	= "\\OrderManager\\";

	// create client window
	CConnectionFrame	*pNewFrame;
	CArcaDoc			*pDoc;
	POSITION			pos;

	pDoc = (CArcaDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CArcaView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}

	// create exchange window
	pDoc = (CArcaDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Exchange Connection");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pExchangeView = (CArcaView *)pDoc->GetNextView(pos);
		m_pExchangeView->CreateTimer(15000);
	}

	// create error, status, and traffic windows
	CTrafficDoc	*pTrafficDoc;

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

	CFileException fe;
	CString Path;

	GetPrivateProfileString("Log File Information",	
							"Path",	
							m_Path,	
							Path.GetBuffer(128), 
							128, 
							m_DatFile); 
	Path.ReleaseBuffer();
	
	m_pServer	= new CServer();
	m_pStatus	= new CArcaStatus();

	std::string datFile((LPCSTR) pApp->DatFile());

	try 
	{
		m_pExchange	= new CTerraNovaSession();					// create fix session
	}
	catch(CFixSessionException&) 
	{
		ProcessErrorEx(MODE_ERR_NOTIFY,"TNTO App", "Initialization of FIX session failed.");
		return FALSE;
	}

	CConnection* host = (CConnection*) CreateHost();

	AddHost(host);

	m_pHandler	= new CArcaHandler((CArcaStatus&) *m_pStatus, *host, (CTerraNovaSession&) *m_pExchange, m_ErrorHandler, datFile);

	m_pServer->StartListening();				// start listening for client connections

	// init static vars
	COrder::Init();
	UpdateRunDate();
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();

	return TRUE;
}

BOOL CArcaApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
{
	CLogin Login;

	pC.m_Name = CSV.Get(CSV_TRADER);

	//if the connect is already there, kick it out
	CBaseConnection* pExist = Clients().FindClient((LPCSTR) (pC.m_Name));
	if(pExist)
	{
		CString msg = pExist->GetConnector() + " replaced by " + pC.GetConnector();
		ProcessLogEntry(pC.m_Name,msg);
		ProcessClientLogout(*pExist, CSV);
	}
	
	pC.SetLogonStatus(TRUE);

	Clients().SetClient(&pC, pC.m_Name);

	SetClientConnection(&pC);

	Login.SetTrader(m_Name);
	pC.SendMessage(Login.Message());

	Clients().Dump(pC.Name());

	return TRUE;
}

BOOL CArcaApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();

	Clients().Update();

	return TRUE;
}

BOOL CArcaApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	COrder *pOrder;
	pOrder = new COrder(CSV);
	Handler().ProcessMessage(*pOrder);
	delete pOrder;

	return TRUE;
}

BOOL CArcaApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CCancel *pCancel;
	pCancel = new CCancel(CSV);
	Handler().ProcessMessage(*pCancel);
	delete pCancel;

	return TRUE;
}

BOOL CArcaApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CReplace *pReplace;
	pReplace = new CReplace(CSV);
	Handler().ProcessMessage(*pReplace);
	delete pReplace;

	return TRUE;
}

BOOL CArcaApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CRequest *pRequest;
	pRequest = new CRequest(CSV);
	Handler().ProcessMessage(*pRequest);
	delete pRequest;

	return TRUE;
}

void CArcaApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (!(nErrorCode))
	{
		UpdateHost();

		if (Exchange().StartLogon())
		{
			if (!(Exchange().Logon())) 
				ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}
	}
	else
	{}
}

// App command to run the dialog
void CArcaApp::OnAppAbout()
{
	try
	{
		CAboutDlg aboutDlg;

		CString version;
		CString copyRight;

		GetAboutBoxData(version, copyRight);

		aboutDlg.SetAboutData(m_Name, version, copyRight);

		aboutDlg.DoModal();
	}
	catch(std::exception& e)
	{
		CHAR szBuf[80]; 
 
		sprintf(szBuf, "Error Displaying About Info: %s\n", e.what()); 

		AfxMessageBox(szBuf);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CArcaApp commands

void CArcaApp::AddHost(CConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CArcaApp::DeleteHost(CConnection *pHost)
{
	if (pHost)
	{
		if (pHost->m_hSocket != INVALID_SOCKET)
		{
			pHost->AsyncSelect(0);
			pHost->Close();
		}

		m_pExchangeView->m_List.Delete(pHost);
		delete pHost;
		pHost = NULL;
	}
}

void CArcaApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}
	
void CArcaApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CArcaApp::ProcessErrorEx(DWORD Mode, const CString& Name, const CString& Error)
{
	CMessageApp::ProcessErrorEx(Mode, Name, Error);
	/*
	|| Send the message to the screen.
	*/
	if (m_pErrorView) 
	{
		m_pErrorView->Add(Name + '\t' + Error);
	}
}

void CArcaApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	if (m_bDisplayTraffic) 
	{
		CMessageApp::ProcessDebug(Name, Debug, Append);

		if( m_pTrafficView)
			m_pTrafficView->Add(Name + '\t' + Debug);
	}
}

void CArcaApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CArcaApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;
	
	return CMessageApp::ExitInstance();
}

void CArcaApp::OnResetOutgoing()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CArcaApp::OnResetIncoming()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CArcaApp::OnClearOutgoing()
{
	((CFixSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

void CArcaApp::OnClearIncoming()
{
	((CFixSession&)Exchange()).SetNextIncomingSeqNum(1);
}

void CArcaApp::OnLogout() 
{
	Exchange().Logout();
}

void CArcaApp::UpdateClients()
{
	if (m_pClientView)
		m_pClientView->m_List.Update();
}