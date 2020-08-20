// Island.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Island.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "IslandDoc.h"
#include "IslandView.h"
#include "ConnectionFrame.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "SeqDlg.h"

#include "IslandMessage.h"

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
// CIslandApp

BEGIN_MESSAGE_MAP(CIslandApp, CWinApp)
	//{{AFX_MSG_MAP(CIslandApp)
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
// CIslandApp construction

CIslandApp::CIslandApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIslandApp object

CIslandApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIslandApp initialization

BOOL CIslandApp::InitInstance()
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
		IDR_ISLANDTYPE,
		RUNTIME_CLASS(CIslandDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CIslandView));
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
		m_Name.Format("ISLD_%s", p2 + 1);
	}
	else
	{
		m_Name = "ISLD";
	}

	//m_Name = "ISLD";
	m_DatFile = "\\OrderManager\\" + m_Name + ".dat";
	if(!CMessageApp::InitInstance())
		return FALSE;
	m_Path = "\\OrderManager\\";

	GetPrivateProfileString("Island Information", "MMID", "STAF", m_MMID.GetBuffer(128), 128,	m_DatFile); m_MMID.ReleaseBuffer();

	// create client window
	CConnectionFrame	*pNewFrame;
	CIslandDoc			*pDoc;
	POSITION			pos;

	pDoc = (CIslandDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CIslandView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}

	// create exchange window
	pDoc = (CIslandDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Exchange Connection");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pExchangeView = (CIslandView *)pDoc->GetNextView(pos);
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
							m_Path + m_Name + ".DAT"); 

	

	Path.ReleaseBuffer();
	/*
	if ((m_Log.Open(Path + m_Name + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, 
		&fe)))
	{
		m_Log.SeekToEnd();
	}

	if ((m_ErrorLog.Open(Path + m_Name + "ERROR" + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, 
		&fe)))
	{
		m_ErrorLog.SeekToEnd();
	}

	if ((m_TrafficLog.Open(Path + m_Name + "TRAFFIC" + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", 
		CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, 
		&fe)))
	{
		m_TrafficLog.SeekToEnd();
	}
	*/		
	//m_pDb					= new CCommonDatabase(m_Path + m_Name + ".DAT");	// create database
	//m_pDb->Connect();
	m_pServer				= new CServer();									// create server
	m_pExchange				= new CIslandSession();								// create fix session
	m_pHandler				= new CIslandHandler();								// create order handler
	m_pStatus				= new CIslandStatus();								// create status handler
	AddHost((CHostConnection*)CreateHost());														// connection to exchange
	//AddHost(CreateRecorder());													// connection to db recorder
	m_pServer->StartListening();												// start listening for client connections

	// init static vars
	COrder::Init();
	CIslandMessage::Init();
		
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();

	UpdateRunDate();
	return TRUE;
}

BOOL CIslandApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
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

BOOL CIslandApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();

	Clients().Update();

	return TRUE;
}

BOOL CIslandApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;
	
	COrder order(CSV);
	order.SetOwner(pC.m_Name);
	order.SetSourceId(order.Trader());
	Handler().ProcessMessage(order);
	return TRUE;
}

BOOL CIslandApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CCancel cancel(CSV);
	cancel.SetOwner(pC.m_Name);
	cancel.SetSourceId(cancel.Trader());
	Handler().ProcessMessage(cancel);

	return TRUE;
}

BOOL CIslandApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;
    CString owner = CSV.Get(CSV_TRADE_FOR);
	CReplace replace(CSV);
	replace.SetOwner(pC.m_Name);
	replace.SetSourceId(replace.Trader());
	Handler().ProcessMessage(replace);

	return TRUE;
}

BOOL CIslandApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CRequest request(CSV);
	request.SetOwner(pC.m_Name);
	request.SetSourceId(request.Trader());
	Handler().ProcessMessage(request);

	/*
	CRequest *pRequest;
	pRequest = new CRequest(CSV);
	pRequest->SetOwner(pC.m_Name);
	pRequest->SetSourceId(pRequest->Trader());
	Handler().ProcessMessage(*pRequest);
	delete pRequest;
	*/

	return TRUE;
}

void CIslandApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (!(nErrorCode))
	{
		pC.SetLogonStatus(TRUE);
		UpdateHost();

		if (Exchange().StartLogon())
		{
			if (!(Exchange().Logon())) 
				ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}
	}
	else
	{}
	//	pC.PostError(nErrorCode);
}

// App command to run the dialog
void CIslandApp::OnAppAbout()
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
// CIslandApp commands

void CIslandApp::AddHost(CConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CIslandApp::DeleteHost(CConnection *pHost)
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

void CIslandApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}
	

void CIslandApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CIslandApp::ProcessErrorEx(DWORD Mode, const CString& Name, const CString& Error)
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

void CIslandApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	if (m_bDisplayTraffic) 
	{
		CMessageApp::ProcessDebug(Name, Debug, Append);

		if( m_pTrafficView)
			m_pTrafficView->Add(Name + '\t' + Debug);
	}
}

void CIslandApp::ProcessTimer()
{
	/*if ((m_pRecorder->Reconnect()) && (!m_pRecorder->Connected()))
	{
		ProcessError("Recorder", "Connection To Host Lost");
		RecycleRecorder();
	}*/
}

void CIslandApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CIslandApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;
		
	return CWinApp::ExitInstance();
}

void CIslandApp::OnLogout() 
{
	Exchange().Logout();
}

void CIslandApp::UpdateClients()
{
	if (m_pClientView)
		m_pClientView->m_List.Update();
}


void CIslandApp::OnResetOutgoing()
{
	CSeqDlg dlg(TRUE);
	if(dlg.DoModal() == IDOK)
		((CIslandSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CIslandApp::OnResetIncoming()
{
	CSeqDlg dlg(FALSE);
	if(dlg.DoModal() == IDOK)
		((CIslandSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CIslandApp::OnClearOutgoing()
{
	((CIslandSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

void CIslandApp::OnClearIncoming()
{
	((CIslandSession&)Exchange()).SetNextIncomingSeqNum(1);
}
