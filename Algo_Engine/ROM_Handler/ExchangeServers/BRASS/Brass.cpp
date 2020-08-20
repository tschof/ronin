// Brass.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Brass.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "BrassDoc.h"
#include "BrassView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "BrassFixSession.h"
#include "SeqDlg.h"
#include "BrassReplace.h"

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
// CBrassApp

BEGIN_MESSAGE_MAP(CBrassApp, CWinApp)
	//{{AFX_MSG_MAP(CBrassApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_LOGOUT, OnLogout)
	ON_COMMAND(IDM_RESET_OUTGOING, OnResetOutgoing)
	ON_COMMAND(IDM_RESET_INCOMING, OnResetIncoming)
	ON_COMMAND(IDM_CLEAR_OUTGOING, OnClearOutgoing)
	ON_COMMAND(IDM_CLEAR_INCOMING, OnClearIncoming)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrassApp construction

CBrassApp::CBrassApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBrassApp object

CBrassApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBrassApp initialization

BOOL CBrassApp::InitInstance()
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
		IDR_BRASSTYPE,
		RUNTIME_CLASS(CBrassDoc),
		RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
		RUNTIME_CLASS(CBrassView));
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

	//m_Path	= "\\OrderManager\\";
	char pathbuf[256];
	GetModuleFileName(NULL, pathbuf, 256);
	char* p1 = strrchr(pathbuf, '.');
	char* p2 = strrchr(pathbuf, '_');
	*p1 = '\0';
	
	if (p2 != NULL)
	{
		m_Name.Format("BRASS_%s", p2 + 1);
	}
	else
	{
		m_Name = "BRASS";
	}
	//m_Name	= "BRASS";
	m_DatFile = "\\Ordermanager\\" + m_Name + ".dat";
	if(!CMessageApp::InitInstance())
		return FALSE;

	// create client window
	CConnectionFrame	*pNewFrame;
	CBrassDoc			*pDoc;
	POSITION			pos;

	pDoc = (CBrassDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CBrassView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}

	// create exchange window
	pDoc = (CBrassDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Exchange Connection");
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->Restore();
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pExchangeView = (CBrassView *)pDoc->GetNextView(pos);
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
	
	m_pDb = new CCommonDatabase(m_Path + m_Name + ".DAT");	// create database
	m_pDb->Connect();
	m_pServer	= new CServer();									// create server

	m_pStatus	= new CBrassStatus();						// create status handler
	AddHost((CBaseConnection*) CreateHost());				// connection to exchange

#ifdef _RECORDER
	AddHost((CHostConnection*)CreateRecorder());			// connection to db recorder
#endif

	m_pHandler	= new CBrassHandler(*m_pHost, std::string((LPCSTR) pApp->DatFile())); // create order handler

	try 
	{
		m_pExchange	= new CBrassFixSession();								// create fix session
	}
	catch(CFixSessionException&) 
	{
		ProcessErrorEx(MODE_ERR_NOTIFY,"Brass App", "Initialization of FIX session failed.");
		return FALSE;
	}

	m_pServer->StartListening();							// start listening for client connections

	// init static vars
	COrder::Init();
	UpdateRunDate();
	
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();
	pMainFrame->CreateTimer(5000);

	return TRUE;
}

BOOL CBrassApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
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

//	CString s = "E,084150,CMSTDCAN,MCC_2118_185002,2,NSM,100,16.78,4,0,A,0551,43834320D0,25,0,TU70,N,OTTO-20040730-2660,0,15,0,0,TU70,WAG,TU70,,,OTTO,1,,,,,,,E,,,,,,25,,,,,,,,,084150,,,,,,,G,,";
//	CCsv csv(s);
//	COrder order(csv);
//	((CBrassHandler*)m_pHandler)->Process(order);
/*
	s = "R,084150,TU70,MCC_2118_185002,2,NSM,80,15.78,4,1,A,0551,43834320D0,25,0,TU70,N,OTTO-20040730-2660,0,10,0,0,TU70,WAG,TU70,,,OTTO,1,,,,,,,E,,,,,,25,,,,,,,,,084150,,,,,,,G,,";
	CCsv sv(s);
	CReplace Replace(sv);
	((CBrassHandler*)m_pHandler)->Process(Replace);
*/
	return TRUE;
}

BOOL CBrassApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();
	Clients().Update();

	return TRUE;
}

BOOL CBrassApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
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

BOOL CBrassApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
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

BOOL CBrassApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CReplace *pReplace;
	pReplace = new CReplace(CSV);
	pReplace->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pReplace->Trader());
	pReplace->SetSourceId(SourceId);
	((CBrassHandler*)&Handler())->ProcessMessage(*pReplace);
	delete pReplace;

	return TRUE;
}

BOOL CBrassApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
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

void CBrassApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
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
	//else
	//	pC.PostError(nErrorCode);
}

// App command to run the dialog
void CBrassApp::OnAppAbout()
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
// CBrassApp commands

void CBrassApp::AddHost(CBaseConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CBrassApp::DeleteHost(CHostConnection *pHost)
{
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
			ProcessErrorEx(MODE_ERR_DEVELOPER,"COrderManagerApp::DeleteHost", "Exception!");
		}
		pHost = NULL;
	}
}

void CBrassApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}
	
void CBrassApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	if (m_pStatusView) 
		if (::IsWindow(m_pStatusView->GetSafeHwnd()))
			m_pStatusView->Add(Name + '\t' + Message);
}

void CBrassApp::ProcessErrorEx(DWORD Mode, const CString& Name, const CString& Error)
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

void CBrassApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	CMessageApp::ProcessDebug(Name, Debug, Append);

	if (m_pTrafficView) 
	{
		m_pTrafficView->Add(Name + '\t' + Debug);
	}
}

void CBrassApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}

int CBrassApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pTrafficView = NULL;
	
	return CMessageApp::ExitInstance();
}

void CBrassApp::OnResetOutgoing()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CBrassApp::OnResetIncoming()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		((CFixSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CBrassApp::OnClearOutgoing()
{
	((CFixSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

void CBrassApp::OnClearIncoming()
{
	((CFixSession&)Exchange()).SetNextIncomingSeqNum(1);
}

void CBrassApp::OnLogout() 
{
	Exchange().Logout();
}

void CBrassApp::UpdateClients()
{
	if (m_pClientView)
		m_pClientView->m_List.Update();
}


void CBrassApp::ProcessTimer()
{
#ifdef _RECORDER
	if ((m_pRecorder->Reconnect()) && (!m_pRecorder->Connected()))
	{
		ProcessErrorEx(MODE_ERR_NOTIFY,"Recorder", "Connection To Host Lost");
		RecycleRecorder();
	}
#endif
}