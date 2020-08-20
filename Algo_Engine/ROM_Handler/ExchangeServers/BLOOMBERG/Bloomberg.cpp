
// Bloomberg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Bloomberg.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "BloombergDoc.h"
#include "BloombergView.h"
#include "ConnectionFrame.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "BloombergFixSession.h"
#include "BloombergHandler.h"
#include "BloombergStatus.h"
#include "SeqDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	void SetAboutData(CString& version, CString& copyRight)
	{
		m_version	= version;
		m_copyRight = copyRight;
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

	m_title += AfxGetAppName();

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
// CBloombergApp

BEGIN_MESSAGE_MAP(CBloombergApp, CWinApp)
	//{{AFX_MSG_MAP(CBloombergApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_LOGOUT, OnLogout)
	ON_COMMAND(IDM_RESET_OUTGOING, OnResetOutgoing)
	ON_COMMAND(IDM_RESET_INCOMING, OnResetIncoming)
	ON_COMMAND(IDM_CLEAR_OUTGOING, OnClearOutgoing)
	ON_COMMAND(IDM_CLEAR_INCOMING, OnClearIncoming)
	ON_COMMAND(IDM_TAGGLE_DISPLAY, OnTaggleDisplay)
	ON_UPDATE_COMMAND_UI(IDM_TAGGLE_DISPLAY, OnUpdateTaggleDisplay)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!

	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBloombergApp construction

CBloombergApp::CBloombergApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bDisplayTraffic = false;
	m_nReConnection = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBloombergApp object

CBloombergApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBloombergApp initialization

BOOL CBloombergApp::InitInstance()
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
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_BLOOMBTYPE,
		RUNTIME_CLASS(CBloombergDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CBloombergView));
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

	//m_Path = "C:\\OrderManager\\";
	char pathbuf[256];
	GetModuleFileName(NULL, pathbuf, 256);
	char* p1 = strrchr(pathbuf, '.');
	char* p2 = strrchr(pathbuf, '_');
	*p1 = '\0';
	
	if (p2 != NULL)
	{
		m_Name.Format("Bloomberg_%s", p2 + 1);
	}
	else
	{
		m_Name = "Bloomberg";
	}

	//m_Name = "Bloomberg";
	m_DatFile = "c:\\Ordermanager\\" + m_Name + ".dat";
	if(!CMessageApp::InitInstance())
		return FALSE;

	// create client window
	CConnectionFrame	*pNewFrame;
	CBloombergDoc		*pDoc;
	POSITION				pos;

	pDoc = (CBloombergDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CBloombergView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}

	// create exchange window
	pDoc = (CBloombergDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Exchange Connection");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pExchangeView = (CBloombergView *)pDoc->GetNextView(pos);
		m_pExchangeView->CreateTimer(15000);

		// Get Max reconnection set and set connection check timer
		char reConnection[3];
		GetPrivateProfileString("Bloomberg Connection", "Maximun Reconnection",	"3", reConnection, 3, DatFile()); 
		m_nMaxReConnection = atoi(reConnection);
		//m_pExchangeView->CreateConnectionCheckTimer(15000);
	}

	GetPrivateProfileString("Fix Information", "SenderSubId",	"", m_SenderSubId.GetBuffer(128), 128, DatFile()); m_SenderSubId.ReleaseBuffer();
	
	// create error, status, and traffic windows
	CTrafficDoc *pTrafficDoc;

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

	m_pServer				= new CServer();
	m_pHandler				= new CBloombergHandler();

	try {
		m_pExchange	= new CBloombergFixSession();							// create fix session
	}
	catch(CFixSessionException&) {
		ProcessErrorEx(MODE_ERR_NOTIFY,"Bloomberg App", "Initialization of FIX session failed.");
		return FALSE;
	}

	m_pStatus				= new CBloombergStatus();							// create status handler
	AddHost((CHostConnection*) CreateHost());

							
	m_pServer->StartListening();												// start listening for client connections

	// init static vars
	COrder::Init();
	UpdateRunDate();
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();

//	UpdateRunDate();

	return TRUE;
}

BOOL CBloombergApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
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

	Login.SetTrader(m_Name);
	pC.SendMessage(Login.Message());
	Clients().Dump(pC.Name());

	return TRUE;
}


BOOL CBloombergApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();
	Clients().Update();

	return TRUE;
}

BOOL CBloombergApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;
	
	COrder *pOrder;
	pOrder = new COrder(CSV);
	pOrder->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pOrder->Trader());
	//pOrder->SetSourceId("845544");
	pOrder->SetSourceId(SourceId);
	Handler().ProcessMessage(*pOrder);
	delete pOrder;

	return TRUE;
}

BOOL CBloombergApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CCancel *pCancel;
	pCancel = new CCancel(CSV);
	pCancel->SetOwner(pC.m_Name);
	//SourceId.Format("%s,%s", pC.m_Name, pCancel->Trader());
	//pCancel->SetSourceId(SourceId);
	//pCancel->SetSourceId("845544");
	pCancel->SetSourceId(SenderSubId());
	Handler().ProcessMessage(*pCancel);
	delete pCancel;

	return TRUE;
}

BOOL CBloombergApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CReplace *pReplace = new CReplace(CSV);
	pReplace->SetOwner(pC.m_Name);
	//SourceId.Format("%s,%s", pC.m_Name, pReplace->Trader());
	//pReplace->SetSourceId(SourceId);
	//pReplace->SetSourceId("845544");
	pReplace->SetSourceId(SenderSubId());
	Handler().ProcessMessage(*pReplace);
	delete pReplace;

	return TRUE;
}

BOOL CBloombergApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CRequest *pRequest;
	pRequest = new CRequest(CSV);
	pRequest->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pRequest->Trader());
	pRequest->SetSourceId(SourceId);
	//pRequest->SetSourceId("845544");
	Handler().ProcessMessage(*pRequest);
	delete pRequest;

	return TRUE;
}

void CBloombergApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (!(nErrorCode))
	{
		//AddHost(m_pHost);
		//m_pHost->SetLogonStatus(TRUE);
		UpdateHost();
		//m_nReConnection = 0;

		if (Exchange().StartLogon())
		{
			if (!(Exchange().Logon())) 
				ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}
	}
	else
	{ }
		//m_pHost->PostError(nErrorCode);
	//}
}


void CBloombergApp::ProcessConnectionCheck()
{
	if (m_pHost->Connected() != TRUE)
	{
		if (m_nReConnection < m_nMaxReConnection)
		{
			// Let FixSession do the reconnection
			m_nReConnection++;
		}
		else
		{
			// Assume that the server machine has been switched 
			// to the backup(or primary) one.
			// Switch IP address and reset the m_nReConnection
			DeleteHost(m_pHost);
			SwitchHostIP();
			m_nReConnection = 0;
			AddHost((CHostConnection*) CreateHost());
		}
	}
}


void CBloombergApp::AddHost(CConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CBloombergApp::DeleteHost(CConnection *pHost)
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

void CBloombergApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}

void CBloombergApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CBloombergApp::ProcessErrorEx(DWORD Mode,const CString& Name, const CString& Error)
{
	CMessageApp::ProcessErrorEx(Mode,Name, Error);
	
	if (m_pErrorView) 
	{
		m_pErrorView->Add(Name + '\t' + Error);
	}
}

void CBloombergApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	if (m_bDisplayTraffic) 
	{
		CMessageApp::ProcessDebug(Name, Debug, Append);

		if( m_pTrafficView)
			m_pTrafficView->Add(Name + '\t' + Debug);
	}
}


void CBloombergApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CBloombergApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;

	return CMessageApp::ExitInstance();
}

void CBloombergApp::OnLogout() 
{
	Exchange().Logout();
}

void CBloombergApp::OnResetOutgoing()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CBloombergApp::OnResetIncoming()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CBloombergApp::OnClearOutgoing()
{
	((CFixSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

void CBloombergApp::OnClearIncoming()
{
	((CFixSession&)Exchange()).SetNextIncomingSeqNum(1);
}
	
void CBloombergApp::UpdateClients()
{
	if (m_pClientView)
		m_pClientView->m_List.Update();
}

void CBloombergApp::SwitchHostIP()
{
	char pHostIP[32], bHostIP[32];

	GetPrivateProfileString(m_Name + " Connection", "Host", "", pHostIP, 31, DatFile()); 
	GetPrivateProfileString(m_Name + " Connection", "Backup Host", "", bHostIP, 31, DatFile()); 
	WritePrivateProfileString(m_Name + " Connection", "Host", bHostIP, DatFile());
	WritePrivateProfileString(m_Name + " Connection", "Backup Host", pHostIP, DatFile());
	//m_pHost->m_Host = bHostIP;
}

// App command to run the dialog
void CBloombergApp::OnAppAbout()
{
	try
	{
		CAboutDlg aboutDlg;

		CString version;
		CString copyRight;

		GetAboutBoxData(version, copyRight);

		aboutDlg.SetAboutData(version, copyRight);

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
// CBloombergApp message handlers

