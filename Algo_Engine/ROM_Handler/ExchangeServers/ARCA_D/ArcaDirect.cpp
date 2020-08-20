/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  : Defines the class behaviors for the application.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *     Parnell     
 *
 *******************************************************************************/// ArcaDirect.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ArcaDirect.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "ArcaDirectDoc.h"
#include "ArcaDirectView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "ArcaDirectSession.h"
#include "DirectConnection.h"
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
    m_version		= "1.0.3";
    m_copyRight	= "Copyright (c) 2005 by RONIN CAPITAL, LLC";
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
// CArcaDirectApp

    BEGIN_MESSAGE_MAP(CArcaDirectApp, CWinApp)
    //{{AFX_MSG_MAP(CArcaDirectApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(IDM_LOGOUT, OnLogout)
    ON_COMMAND(IDM_RESET_OUTGOING, OnResetOutgoing)
    ON_COMMAND(IDM_RESET_INCOMING, OnResetIncoming)
    ON_COMMAND(IDM_CLEAR_OUTGOING, OnClearOutgoing)
    ON_COMMAND(IDM_CLEAR_INCOMING, OnClearIncoming)
	ON_COMMAND(IDM_RESET_ORDER_ID, OnResetOrderId)
	//}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectApp construction

    CArcaDirectApp::CArcaDirectApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CArcaDirectApp object

CArcaDirectApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectApp initialization

BOOL CArcaDirectApp::InitInstance()
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
        IDR_BRUTTYPE,
        RUNTIME_CLASS(CArcaDirectDoc),
        RUNTIME_CLASS(CConnectionFrame), // custom MDI child frame
        RUNTIME_CLASS(CArcaDirectView));
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
        m_Name.Format("ArcaDirect_%s", p2 + 1);
    }
    else
    {
        m_Name = "ArcaDirect";
    }
    //m_Name = "ArcaDirect";

    m_DatFile = "c:\\Ordermanager\\" + m_Name + ".dat";
    //m_Name = "ArcaDirect";
    //m_DatFile = "\\Ordermanager\\ArcaDirect.dat";
    GetPrivateProfileString("Log File Information",
                            "Log File Path",
                            "c:\\ordermanager\\",
                            m_Path.GetBuffer(128),
                            128, DatFile());

    m_Path.ReleaseBuffer();

    if(!CMessageApp::InitInstance())
        return FALSE;
    //m_Path = "\\OrderManager\\";

    // create client window
    CConnectionFrame	*pNewFrame;
    CArcaDirectDoc			*pDoc;
    POSITION			pos;

    pDoc = (CArcaDirectDoc *)pDocTemplate->CreateNewDocument();
    pNewFrame = (CConnectionFrame *)(
        pDocTemplate->CreateNewFrame(pDoc, NULL));
    pNewFrame->SetTitle("Client Connections");
    pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
    pNewFrame->Restore();
    if ((pos = pDoc->GetFirstViewPosition()))
    {
        m_pClientView = (CArcaDirectView *)pDoc->GetNextView(pos);
        m_Clients.SetList(&m_pClientView->m_List);
    }

    // create exchange window
    pDoc = (CArcaDirectDoc *)pDocTemplate->CreateNewDocument();
    pNewFrame = (CConnectionFrame *)(
        pDocTemplate->CreateNewFrame(pDoc, NULL));
    pNewFrame->SetTitle("Exchange Connection");
    pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
    pNewFrame->Restore();
    if ((pos = pDoc->GetFirstViewPosition()))
    {
        m_pExchangeView = (CArcaDirectView *)pDoc->GetNextView(pos);
        m_pExchangeView->CreateTimer(15000);
    }

    // create error, status, and traffic windows
    CTrafficDoc		*pTrafficDoc;

    pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
    pNewFrame = (CConnectionFrame *)(
        pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
    pNewFrame->SetTitle("Order Traffic");
    pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
    pNewFrame->Restore();
    if ((pos = pTrafficDoc->GetFirstViewPosition()))
        m_pTrafficView = (CTrafficView *)pTrafficDoc->GetNextView(pos);

    pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
    pNewFrame = (CConnectionFrame *)(
        pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
    pNewFrame->SetTitle("Status Messages");
    pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
    pNewFrame->Restore();
    if ((pos = pTrafficDoc->GetFirstViewPosition()))
        m_pStatusView = (CTrafficView *)pTrafficDoc->GetNextView(pos);

    pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
    pNewFrame = (CConnectionFrame *)(
        pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
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
	
    //m_pDb		= new CCommonDatabase(m_Path + m_Name + ".DAT");	// create database
    //m_pDb->Connect();
    m_pServer	= new CServer();		// create server
    m_pHandler	= new ArcaDirectHandler();	// create order handler
    m_pExchange	= new ArcaDirectSession();	// create fix session
    m_pStatus	= new ArcaDirectStatus();	// create status handler
	
    //CreateDirectHost();
//    m_pDirect->ConnectToHost();
    // AddHost(m_pDirect);
    AddHost(CreateHost()); // connection to exchange
    
    m_pServer->StartListening();		// start listening for client connections

    // init static vars
    COrder::Init();
    UpdateRunDate();

    // The main window has been initialized, so show and update it.
    pMainFrame->ShowWindow(SW_SHOWNORMAL);
    pMainFrame->Restore();
    pMainFrame->UpdateWindow();

    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientLogon(CBaseConnection &pC,
                                        CCsv& CSV)
{
    CLogin Login;

    pC.m_Name = CSV.Get(CSV_TRADER);

    //if the connect is already there, kick it out
    CBaseConnection* pExist = 
        Clients().FindClient((LPCSTR) (pC.m_Name));
    if(pExist)
    {
        CString msg = 
            pExist->GetConnector() +
            " replaced by " + pC.GetConnector();
        ProcessLogEntry(pC.m_Name,msg);
        ProcessClientLogout(*pExist, CSV);
    }
    pC.SetLogonStatus(TRUE);
    // ((DirectConnection&)pC).SetLoggedOn(TRUE);

    Clients().SetClient(&pC, pC.m_Name);

    Login.SetTrader(m_Name);
    pC.SendMessage(Login.Message());
    Clients().Dump(pC.Name());

    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientLogout(CBaseConnection &pC,
                                         CCsv& CSV)
{
    CLogout Logout;

    Logout.SetTrader(pC.m_Name);
    pC.SendMessage(Logout.Message());

    pC.SetLogonStatus(FALSE);
    pC.Close();
    Clients().Update();

    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientOrder(CBaseConnection& pC,
                                        CCsv& CSV)
{
    COrder order(CSV);
    order.SetOwner(pC.m_Name);
    order.SetSourceId(order.Trader());
    Handler().ProcessMessage(order);
    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientCancel(CBaseConnection& pC,
                                         CCsv& CSV)
{
    CCancel cancel(CSV);
    cancel.SetOwner(pC.m_Name);
    cancel.SetSourceId(cancel.Trader());
    Handler().ProcessMessage(cancel);
    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientReplace(CBaseConnection& pC,
                                          CCsv& CSV)
{
    CString tradeFor = CSV.Get(CSV_TRADE_FOR);
    CReplace replace(CSV);
    replace.SetOwner(pC.m_Name);
    replace.SetSourceId(replace.Trader());
    replace.SetLocalAccount(tradeFor);
    Handler().ProcessMessage(replace);
    return TRUE;
}

BOOL CArcaDirectApp::ProcessClientStatus(CBaseConnection& pC,
                                         CCsv& CSV)
{
    CRequest request(CSV);
    request.SetOwner(pC.m_Name);
    request.SetSourceId(request.Trader());
    Handler().ProcessMessage(request);
    return TRUE;
}

void CArcaDirectApp::OnHostConnect(CBaseConnection &pC,
                                   int nErrorCode)
{
    if (!(nErrorCode))
    {
        if (Exchange().StartLogon())
        {
            if (!(Exchange().Logon())) 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "Error Logging On");
        }
    }
    else {

        if (0 != nErrorCode)
        {
            switch( nErrorCode )
            {
            case WSAEADDRINUSE: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The specified address is already in use.");
                break;
            case WSAEADDRNOTAVAIL: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The specified address is not available from the local machine.");
                break;
            case WSAEAFNOSUPPORT: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "Addresses in the specified family cannot be used with this socket.");
                break;
            case WSAECONNREFUSED: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The attempt to connect was forcefully rejected.");
                break;
            case WSAEDESTADDRREQ: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "A destination address is required.");
                break;
            case WSAEFAULT: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The lpSockAddrLen argument is incorrect.");
                break;
            case WSAEINVAL: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The socket is already bound to an address.");
                break;
            case WSAEISCONN: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The socket is already connected.");
                break;
            case WSAEMFILE: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "No more file descriptors are available.");
                break;
            case WSAENETUNREACH: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The network cannot be reached from this host at this time.");
                break;
            case WSAENOBUFS: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "No buffer space is available. The socket cannot be connected.");
                break;
            case WSAENOTCONN: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The socket is not connected.");
                break;
            case WSAENOTSOCK: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The descriptor is a file, not a socket.");
                break;
            case WSAETIMEDOUT: 
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),
                               "The attempt to connect timed out without establishing a connection. ");
                break;
            default:
                TCHAR szError[256];
                wsprintf(szError, "OnConnect error: %d", nErrorCode);
                ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(),szError);
                break;
            }
        }

    }
    //else
    //	pC.PostError(nErrorCode);
}

// App command to run the dialog
void CArcaDirectApp::OnAppAbout()
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
 
        sprintf(szBuf,
                "Error Displaying About Info: %s\n", e.what()); 

        AfxMessageBox(szBuf);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectApp commands

void CArcaDirectApp::AddHost(CBaseConnection *pHost)
{
    if (pHost)
        m_pExchangeView->m_List.Add(pHost);
}

void CArcaDirectApp::DeleteHost(DirectConnection *pHost)//DirectConnection *pHost)
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

void CArcaDirectApp::UpdateHost()
{
    if (m_pExchangeView)
        m_pExchangeView->m_List.Update();
}
	
void CArcaDirectApp::ProcessLogEntry(const CString& Name,
                                     const CString& Message, char Append)
{
    CMessageApp::ProcessLogEntry(Name, Message, Append);
    if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}



void CArcaDirectApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
    CMessageApp::ProcessTraffic(Name, Debug, Append);
}

void CArcaDirectApp::SetStatusTimer(UINT Interval)
{
    m_pExchangeView->CreateTimer(Interval);
}

int CArcaDirectApp::ExitInstance() 
{
    m_pErrorView = NULL;
    m_pStatusView = NULL;
    m_pTrafficView = NULL;
	
    return CMessageApp::ExitInstance();
}

void CArcaDirectApp::OnResetOutgoing()
{
    CSeqDlg dlg;
    if(dlg.DoModal() == IDOK)
        ((ArcaDirectSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CArcaDirectApp::OnResetIncoming()
{
    CSeqDlg dlg;
    if(dlg.DoModal() == IDOK)
        ((ArcaDirectSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
}

void CArcaDirectApp::OnClearOutgoing()
{
    ((ArcaDirectSession&)Exchange()).SetNextOutgoingSeqNum(-1);
}

void CArcaDirectApp::OnClearIncoming()
{
    ((ArcaDirectSession&)Exchange()).SetNextIncomingSeqNum(-1);
}

void CArcaDirectApp::OnLogout() 
{
    Exchange().Logout();
}

void CArcaDirectApp::UpdateClients()
{
    if (m_pClientView)
        m_pClientView->m_List.Update();
}

void CArcaDirectApp::OnResetOrderId() 
{
	CSeqDlg dlg;
    if(dlg.DoModal() == IDOK)
        ((ArcaDirectSession&)Exchange()).SetClientOrderID(dlg.SeqNum());
	
}
