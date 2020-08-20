// CHX.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CHX.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "CHXDoc.h"
#include "CHXView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "ChxFixSession.h"
#include "SeqDlg.h"
#include "CHXReplace.h"


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
// CCHXApp

BEGIN_MESSAGE_MAP(CCHXApp, CWinApp)
	//{{AFX_MSG_MAP(CCHXApp)
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
// CCHXApp construction

CCHXApp::CCHXApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDisplayTraffic = false;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCHXApp object

CCHXApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCHXApp initialization

BOOL CCHXApp::InitInstance()
{
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
		IDR_CHXTYPE,
		RUNTIME_CLASS(CCHXDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CCHXView));
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

	m_Name	= "CHX";
	m_DatFile = "\\Ordermanager\\" + m_Name + ".dat";
	if(!CMessageApp::InitInstance())
		return FALSE;

	// create client window
	CConnectionFrame	*pNewFrame;
	CCHXDoc			*pDoc;
	POSITION			pos;

	pDoc = (CCHXDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CCHXView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}

	// create exchange window
	pDoc = (CCHXDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Exchange Connection");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pExchangeView = (CCHXView *)pDoc->GetNextView(pos);
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
							DatFile()); 
	Path.ReleaseBuffer();
	
	//m_pDb = new CCommonDatabase(m_Path + m_Name + ".DAT");	// create database
	//m_pDb->Connect();
	m_pServer	= new CServer();									// create server
	m_pHandler	= new CCHXHandler();								// create order handler
	try {
		m_pExchange	= new CChxFixSession();								// create fix session
	}
	catch(CFixSessionException&) {
		ProcessErrorEx(MODE_ERR_NOTIFY,"CHX App", "Initialization of FIX session failed.");
		return FALSE;
	}
	m_pStatus	= new CCHXStatus();								// create status handler
	AddHost((CHostConnection*) CreateHost());						// connection to exchange
	m_pServer->StartListening();									// start listening for client connections

	// init static vars
	COrder::Init();
	UpdateRunDate();
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();

	//m_pExchange->ProcessMessage("8=FIX.4.0.9=230.35=C.49=CHX.56=MLPC.34=96.52=20010629-12:35:21.94=0.33=6.58=EFFECTIVE IMMEDIATELY THE.58=FOLLOWING SYMBOL  BRKS  IS.58=TRADING ON CHX...ALL ORDER FLOW.58=IS APPRECIATED..DISREGARD WIRE.58=ABOUT CANCELLED ORDERS.58=THANK YOU   CHX.10=072.");
	return TRUE;
}

BOOL CCHXApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
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

	Clients().SetClient(&pC, pC.m_Name );

	Login.SetTrader(m_Name);
	pC.SendMessage(Login.Message());

	Clients().Dump(pC.Name());

	return TRUE;
}

BOOL CCHXApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();
	Clients().Update();

	return TRUE;
}

BOOL CCHXApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	COrder *pOrder;
	pOrder = new COrder(CSV);
	pOrder->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pOrder->Trader());
	pOrder->SetSourceId(SourceId);
	Handler().ProcessMessage(*pOrder);
	delete pOrder;

	return TRUE;
}

BOOL CCHXApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CCancel *pCancel;
	pCancel = new CCancel(CSV);
	pCancel->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pCancel->Trader());
	pCancel->SetSourceId(SourceId);
	Handler().ProcessMessage(*pCancel);
	delete pCancel;

	return TRUE;
}

BOOL CCHXApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CReplace *pReplace = new CReplace(CSV);
	pReplace->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pReplace->Trader());
	pReplace->SetSourceId(SourceId);
	Handler().ProcessMessage(*pReplace);
	delete pReplace;

	return TRUE;
}

BOOL CCHXApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CRequest *pRequest;
	pRequest = new CRequest(CSV);
	pRequest->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pRequest->Trader());
	pRequest->SetSourceId(SourceId);
	Handler().ProcessMessage(*pRequest);
	delete pRequest;

	return TRUE;
}

void CCHXApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (!(nErrorCode))
	{
		// pC.SetLogonStatus(TRUE);
		UpdateHost();

		if (Exchange().StartLogon())
		{
			if (!(Exchange().Logon())) 
				ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}
	}
	else
	{}
		//pC.PostError(nErrorCode);
}
// App command to run the dialog
void CCHXApp::OnAppAbout()
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
// CCHXApp commands

void CCHXApp::AddHost(CConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CCHXApp::DeleteHost(CConnection *pHost)
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

void CCHXApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}
	
void CCHXApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CCHXApp::ProcessErrorEx(DWORD Mode, const CString& Name, const CString& Error)
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

void CCHXApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	if (m_bDisplayTraffic) 
	{
		CMessageApp::ProcessDebug(Name, Debug, Append);

		if( m_pTrafficView)
			m_pTrafficView->Add(Name + '\t' + Debug);
	}
}

void CCHXApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CCHXApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;
	
	return CMessageApp::ExitInstance();
}

void CCHXApp::OnResetOutgoing()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CCHXApp::OnResetIncoming()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CCHXApp::OnClearOutgoing()
{
	((CFixSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

void CCHXApp::OnClearIncoming()
{
	((CFixSession&)Exchange()).SetNextIncomingSeqNum(1);
}

void CCHXApp::OnLogout() 
{
	Exchange().Logout();
}

void CCHXApp::UpdateClients()
{
	if (m_pClientView)
		m_pClientView->m_List.Update();
}