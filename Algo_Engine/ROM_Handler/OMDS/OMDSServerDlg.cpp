// OMDSServerDlg.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "OMDSServer.h"
#include "OMDSServerDlg.h"
#include "Connections.h"
#include "Main.h"
#include <fstream>
#include "ConClientMessage.h"
#include "RequestManager.h"
#include "MessageProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

/////////////////////////////////////////////////////////////////////////////
// COMDSServerDlg dialog

COMDSServerDlg::COMDSServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COMDSServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COMDSServerDlg)
	m_nPort = 8088;
	m_szNetwork = _T(";239.202.75.1;239.202.75.2");
	m_szDaemon = _T("tcp:localhost:7500");
	m_szService = _T("7501");
	m_nClients = 0;
	m_nRequests = 0;
	m_nResponses = 0;
	m_nMsgIn = 0;
	m_nMsgOut = 0;
	m_nTibRate = 0;
	m_nTibRatePeak = 0;
	m_szPath = _T("symbol.txt");
	m_nReqNum = 50;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMain = NULL;
	m_bTimer = false;
}

void COMDSServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COMDSServerDlg)
	DDX_Control(pDX, IDC_LIST_REQUEST, m_pWarning);
	DDX_Control(pDX, IDC_LIST_RESPONSE, m_pError);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_NETWORK, m_szNetwork);
	DDX_Text(pDX, IDC_EDIT_DAEMON, m_szDaemon);
	DDX_Text(pDX, IDC_EDIT_SERVICE, m_szService);
	DDX_Text(pDX, IDC_EDIT_CLIENTS, m_nClients);
	DDX_Text(pDX, IDC_EDIT_REQUESTS, m_nRequests);
	DDX_Text(pDX, IDC_EDIT_RESPONSE, m_nResponses);
	DDX_Text(pDX, IDC_EDIT_IN, m_nMsgIn);
	DDX_Text(pDX, IDC_EDIT_OUT, m_nMsgOut);
	DDX_Text(pDX, IDC_EDIT_TIBRATE, m_nTibRate);
	DDX_Text(pDX, IDC_EDIT_TIBRATEPEAK, m_nTibRatePeak);
	DDX_Text(pDX, IDC_EDIT_SYMFILE, m_szPath);
	DDX_Text(pDX, IDC_EDIT_TIBSUB, m_nReqNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COMDSServerDlg, CDialog)
	//{{AFX_MSG_MAP(COMDSServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SIMULATE, OnButtonSimulate)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_CLIENT_UPDATE, OnClientUpdate )
	ON_MESSAGE( WM_REQUEST_UPDATE, OnRequestUpdate )
	ON_MESSAGE( WM_RESPONSE_UPDATE, OnResponseUpdate )
	ON_MESSAGE( WM_IN_TRANS, OnInTransUpdate )
	ON_MESSAGE( WM_OUT_TRANS, OnOutTransUpdate )
	ON_MESSAGE( WM_ERROR_UPDATE, OnError )
	ON_MESSAGE( WM_WARNING_UPDATE, OnWarning )
	ON_MESSAGE( WM_CLIENT_CLOSE, OnClientClose )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COMDSServerDlg message handlers

BOOL COMDSServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pMain = CMain::getInstance();
	m_pMain->SetHwnd(this->GetSafeHwnd()); 
	m_nPort = atoi(m_pMain->m_config.getValue("clientPort").c_str());
	m_szService = m_pMain->m_config.getValue("tibServicePrimary").c_str();
	m_szNetwork = m_pMain->m_config.getValue("tibNetworkPrimary").c_str();
	m_szDaemon = m_pMain->m_config.getValue("tibDaemonPrimary").c_str();
	UpdateData( FALSE );
	m_pMain->m_config.setValue("tibServicePrimary", (LPCSTR)m_szService);
	m_pMain->m_config.setValue("tibNetworkPrimary", (LPCSTR)m_szNetwork);
	m_pMain->m_config.setValue("tibDaemonPrimary", (LPCSTR)m_szDaemon);
	char szPort[8];
	sprintf(szPort, "%d", m_nPort);
	m_pMain->m_config.setValue("clientPort", (LPCSTR)szPort);

	SetTimer(1, 1000, NULL);

	if( !m_pMain->m_config.getDWORD("AutoStart") )
	{
		CWnd* pStopButton = GetDlgItem(IDC_BUTTON_STOP);
		pStopButton->EnableWindow(FALSE);
	}
	else
	{
		OnButtonStart();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COMDSServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void COMDSServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COMDSServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COMDSServerDlg::OnButtonStart()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if( NULL == m_pMain )
	{
		m_pMain = CMain::getInstance();
		m_pMain->SetHwnd(this->GetSafeHwnd()); 
		m_nPort = atoi(m_pMain->m_config.getValue("clientPort").c_str());
		m_szService = m_pMain->m_config.getValue("tibServicePrimary").c_str();
		m_szNetwork = m_pMain->m_config.getValue("tibNetworkPrimary").c_str();
		m_szDaemon = m_pMain->m_config.getValue("tibDaemonPrimary").c_str();
		UpdateData( FALSE );
		m_pMain->m_config.setValue("tibServicePrimary", (LPCSTR)m_szService);
		m_pMain->m_config.setValue("tibNetworkPrimary", (LPCSTR)m_szNetwork);
		m_pMain->m_config.setValue("tibDaemonPrimary", (LPCSTR)m_szDaemon);
		char szPort[8];
		sprintf(szPort, "%d", m_nPort);
		m_pMain->m_config.setValue("clientPort", (LPCSTR)szPort);
	}
	CWnd* pStartButton = GetDlgItem(IDC_BUTTON_START);
	pStartButton->EnableWindow(FALSE);

	m_pMain->run();

	CWnd* pStopButton = GetDlgItem(IDC_BUTTON_STOP);
	pStopButton->EnableWindow(TRUE);
}

void COMDSServerDlg::OnDestroy()
{
	// TODO: Add your message handler code here
	if( m_pMain )
	{
		m_pMain->m_log.write(LOG_MESSAGE, "Application Shutdown.");
		m_pMain->shutDown();
		m_pMain = NULL;
	}

	CDialog::OnDestroy();
}

void COMDSServerDlg::OnButtonStop()
{
	// TODO: Add your control notification handler code here
	if( AfxMessageBox( "Do you really want to stop?", MB_YESNO ) != IDYES )
		return;
	if( m_bTimer )
	{
		KillTimer( 2 );
		m_bTimer = false;
		GetDlgItem( IDC_BUTTON_SIMULATE )->SetWindowText("Go");
	}

	if( m_pMain )
	{
		m_pMain->m_log.write(LOG_MESSAGE, "============== Application Stopping, Please Wait... ==============");

		CWnd* pStartButton = GetDlgItem(IDC_BUTTON_STOP);
		pStartButton->EnableWindow(FALSE);
		m_pMain->m_log.write(LOG_MESSAGE, "============== Application Stopped, You may click on START to restart it. ==========");
		m_pMain->shutDown();
		m_pMain = NULL;
	}

	CWnd* pStartButton = GetDlgItem(IDC_BUTTON_START);
	pStartButton->EnableWindow(TRUE);
}

LRESULT COMDSServerDlg::OnClientUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nClients = wParam;
	UpdateData( FALSE );
	return 0;
}

LRESULT COMDSServerDlg::OnRequestUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nRequests = wParam;
	UpdateData( FALSE );
	return 0;
}

LRESULT COMDSServerDlg::OnResponseUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nResponses = wParam;
	UpdateData( FALSE );
	return 0;
}

LRESULT COMDSServerDlg::OnInTransUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nMsgIn = wParam;
	UpdateData( FALSE );
	return 0;
}

LRESULT COMDSServerDlg::OnOutTransUpdate( WPARAM wParam, LPARAM lParam )
{
	m_nMsgOut = wParam;
	UpdateData( FALSE );
	return 0;
}

LRESULT COMDSServerDlg::OnError( WPARAM wParam, LPARAM lParam )
{
	std::string szError;
	if( !m_pMain )
		return -1;
	if( m_pError.GetCount() > 1000 )
		m_pError.ResetContent();
	while( m_pMain->GetNextError( szError ) )
	{
		if ( szError.size() > 0 )
			m_pError.AddString( szError.c_str() );
	}

	return 0;
}

LRESULT COMDSServerDlg::OnWarning( WPARAM wParam, LPARAM lParam )
{
	std::string szWarning;
	if( !m_pMain )
		return -1;
	if( m_pWarning.GetCount() > 1000 )
		m_pWarning.ResetContent();
	while( m_pMain->GetNextWarning( szWarning ) )
	{
		if ( szWarning.size() > 0 )
			m_pWarning.AddString( szWarning.c_str() );
	}

	return 0;
}

void COMDSServerDlg::OnTimer(UINT nIDEvent)
{
	static UINT lastMsgCount		= 0 ;
	if( nIDEvent == 1 )
	{
		return;
		UINT		currentMsgCount		= m_nMsgOut;

		CTime tm = CTime::GetCurrentTime();

		int sec = tm.GetSecond();

		if (currentMsgCount < lastMsgCount)
		{
			lastMsgCount = currentMsgCount ;
			m_nTibRatePeak = 0;
		}
		else
			m_nTibRate = currentMsgCount - lastMsgCount;

		if (m_nTibRate > m_nTibRatePeak)
			m_nTibRatePeak = m_nTibRate;

	//	m_nTibSubject = m_pMain->getMessageProcessor()->getTotalTibSubject();

		UpdateData(FALSE);

		lastMsgCount = currentMsgCount;
	}
	else if( nIDEvent == 2 )
	{
		GenerateRequests();	
	}
}


LRESULT COMDSServerDlg::OnClientClose( WPARAM wParam, LPARAM lParam )
{
	int cid = wParam;
	m_pMain->getConnections()->disconnectClient(cid);
	return 0;
}



void COMDSServerDlg::OnButtonSimulate() 
{
	// TODO: Add your control notification handler code here
	if( m_pWarning )
		m_pWarning.ResetContent();
	return;
	if( m_bTimer )
	{
		KillTimer( 2 );
		m_bTimer = false;
		GetDlgItem( IDC_BUTTON_SIMULATE )->SetWindowText("Go");
		return;
	}
	UpdateData();
	LoadRequests( m_szPath );
	SetTimer( 2, 1000, NULL );
	m_bTimer = true;
	GetDlgItem( IDC_BUTTON_SIMULATE )->SetWindowText("Pause");
}

void COMDSServerDlg::LoadRequests( const CString & szPath )
{
	std::ifstream fs;
	m_ClientRequests.clear();
	try
	{
		std::string szRequest;
		char buff[32];
		fs.open( (LPCSTR)szPath, std::ios::in );
		if( !fs.is_open() )
			return;
		while( !fs.eof() )
		{
			memset( buff, 0, sizeof(buff) );
			fs.getline( buff, sizeof(buff) );
			szRequest = buff;
			m_ClientRequests.push_back( szRequest );
			szRequest = szRequest.replace( 0, 1, "<!" );
			m_ClientRequests.push_back( szRequest );
		}
		fs.close();
	}
	catch(...)
	{
		return;
	}
}

void COMDSServerDlg::GenerateRequests()
{
	// num of requests to generate
	UpdateData();
	int nRequests = m_nReqNum;
	int nClients = 250;
	srand( (unsigned)time( NULL ) );
	int nSize = m_ClientRequests.size();
	if( nSize == 0 )
		return;
	for( int i = 0; i < nRequests; i++ )
	{
		CConClientMessage clientMessage;
		int index = rand();
		index = index%( nSize );
		std::string szRequest = m_ClientRequests[index];
		int cid = rand();
		cid = cid%250;
		clientMessage.message = szRequest;
		clientMessage.m_clientId = cid;
		m_pMain->getMessageProcessor()->getRequestManager()->addRequest( clientMessage );
		index = szRequest.find( "<OS|" );
		if( index != -1 )
		{
			std::string szSymbol = szRequest.substr( index+5, -1 );
			if( m_OS.find( szSymbol ) == m_OS.end() )
				m_OSPending.push_back( szSymbol );
		}
	}
	if( m_OSPending.size() > 2 )
	{
		GenerateOptionQuoteRequest();
	}
}

void COMDSServerDlg::GenerateOptionQuoteRequest()
{
	std::list<std::string>::iterator it;
	std::string szEx[] = { "A", "Y", "X", "W", "P" };
	for( it = m_OSPending.begin(); it != m_OSPending.end(); it++ )
	{
		try
		{
			COptionSeries os = CMain::getInstance()->getDataManager()->getOptionSeries( (*it) );
			m_OS.insert( (*it) );
			tStringExpirationMap::iterator ito;
			for( ito = os.m_expirationMap.begin(); ito != os.m_expirationMap.end(); ito++ )
			{
				tFloatStrikeMap::iterator its;
				for( its = (*ito).second.m_strikeMap.begin(); its != (*ito).second.m_strikeMap.end(); its++ )
				{
					std::string szCall = (*its).second.m_callRic;
					std::string szPut = (*its).second.m_putRic;
					std::string szRequest;
					
					szRequest = "<QOB|"+(*it)+"|"+szCall+">";
					m_ClientRequests.push_back( szRequest );

					szRequest = "<QOB|"+(*it)+"|"+szPut+">";
					m_ClientRequests.push_back( szRequest );
					
					szRequest = "<!QOB|"+(*it)+"|"+szCall+">";
					m_ClientRequests.push_back( szRequest );

					szRequest = "<!QOB|"+(*it)+"|"+szPut+">";
					m_ClientRequests.push_back( szRequest );
					for( int i = 0; i < 5; i++ )
					{
						szRequest = "<QO|"+(*it)+"|"+szCall+"|"+szEx[i]+">";
						m_ClientRequests.push_back( szRequest );

						szRequest = "<QO|"+(*it)+"|"+szPut+"|"+szEx[i]+">";
						m_ClientRequests.push_back( szRequest );

						szRequest = "<!QO|"+(*it)+"|"+szCall+"|"+szEx[i]+">";
						m_ClientRequests.push_back( szRequest );

						szRequest = "<!QO|"+(*it)+"|"+szPut+"|"+szEx[i]+">";
						m_ClientRequests.push_back( szRequest );
					}
				}

			}
			it = m_OSPending.erase( it );
		}
		catch(...)
		{
			continue;
		}
	}
}
static	char szFilter[] = "Symbol File (*.txt)|*.txt|All Files (*.*)|*.*||";

void COMDSServerDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	// pFileDlg...
	CFileDialog dlg(
		TRUE	 /* open */,
		NULL	/* default extension */,
		NULL	/* initial pFilename in name box*/,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter );

	// Getting pFile name
	if (dlg.DoModal()!=IDOK)
		return;

	// OK has been pressed...

	m_szPath = dlg.GetPathName();
	UpdateData(FALSE);
	
}


