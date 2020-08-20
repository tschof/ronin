/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * FixClientDlg.cpp : implementation file 
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 09/28/05    joel        Change default TargetCompID = RON
 *
 *******************************************************************************/

// FixClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FixClient.h"
#include "FixClientDlg.h"

#include "FileStore.h"
#include "MessageStore.h"
#include "SocketInitiator.h"
#include "SessionSettings.h"
#include "SocketInitiator.h"

#include "ClientInitiator.h"
#include "OmExecution.h"

#include <xiosbase>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <map>
#include <assert.h>

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
// CFixClientDlg dialog

CFixClientDlg::CFixClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixClientDlg)
	m_szCount = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFixClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixClientDlg)
	DDX_Control(pDX, IDC_LIST_REPORT, m_lstReport);
	DDX_Text(pDX, IDC_STATIC_COUNT, m_szCount);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFixClientDlg, CDialog)
	//{{AFX_MSG_MAP(CFixClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOGON, OnButtonLogon)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_BUY, OnButtonBuy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixClientDlg message handlers

BOOL CFixClientDlg::OnInitDialog()
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
	m_nMaxLen = 0;
	CDC *pDC = GetDC();
	TEXTMETRIC metrics;
	pDC->GetTextMetrics( &metrics );
	m_nWidth = metrics.tmAveCharWidth;

	time_t t = time(NULL);
	tm * pTM = localtime(&t);
	std::stringstream stm;
	
	stm << "REPORT" << pTM->tm_year+1900 << std::setw(2) << std::setfill('0') << pTM->tm_mon+1 << std::setw(2) << std::setfill('0') << pTM->tm_mday << ".txt";
	TRACE( "%s", stm.str().c_str() );
	m_Log.open( stm.str().c_str(), std::ios_base::out|std::ios_base::app );
	m_Log << "\n---- Program Started ---\n\n";
	m_Log.flush();


	OnButtonLogon();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFixClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFixClientDlg::OnPaint() 
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
HCURSOR CFixClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFixClientDlg::OnButtonLogon() 
{
	// TODO: Add your control notification handler code here
	_beginthread( ThreadProc, 0, this );
}

void CFixClientDlg::ThreadProc( void * pParam )
{
	CFixClientDlg * pApp = (CFixClientDlg*)pParam;
	if( !pApp )
		return;
	try
	{
		std::string szPath = "FixClient.cfg";
		FIX::SessionSettings settings(szPath);
		std::set<FIX::SessionID> ids = settings.getSessions();
		assert( ids.size() > 0 );
		FIX::SessionID id = *ids.begin();
		std::string szSenderCompID = id.getSenderCompID();
		char pszPath[MAX_PATH];
		GetModuleFileName(NULL,pszPath,sizeof(pszPath));
		szPath = pszPath;
		int nPos = szPath.find_last_of('\\');
		szPath = szPath.substr(0, nPos );
		FIX::FileStoreFactory factory(szPath);
		//FIX::MemoryStoreFactory factory;
		//CClientInitiator initiator(*this, factory, settings);
		FIX::SocketInitiator initiator(*pApp,factory,settings);
		initiator.start();
	}
	catch(FIX::ConfigError& e)
	{
		TRACE( "%s", (LPCSTR)e.what() );
	}	
}

void CFixClientDlg::onMessage( const FIX42::ExecutionReport& execRpt, const FIX::SessionID& sessId ) 
{
	std::stringstream stm;
	stm << execRpt.getString();
	CString szText = (LPCSTR)stm.str().c_str();
	m_nMaxLen = max( szText.GetLength(), m_nMaxLen );
	m_lstReport.SetHorizontalExtent( m_nMaxLen*m_nWidth );
	int nIndex = m_lstReport.AddString( szText );
	m_lstReport.SetCurSel( nIndex );
	m_szCount.Format("%d",m_lstReport.GetCount());
	GetDlgItem(IDC_STATIC_COUNT)->SetWindowText(m_szCount);
	try
	{
		COmExecution exe( execRpt );
		m_Log << exe;
		m_Log.flush();
	}
	catch(...)
	{
		m_Log << "CFixClientDlg::onMessage exception: " << stm.str() << std::endl;
		m_lstReport.AddString("COmExecution exception");
	}
}

void CFixClientDlg::onLogon( const FIX::SessionID& sessionID )
{
	SetWindowText("Fix Drop Copy Client -- Logged On" );
	GetDlgItem(IDC_BUTTON_LOGON)->EnableWindow(FALSE);
}

void CFixClientDlg::onLogout( const FIX::SessionID& sessionID )
{
	SetWindowText("Fix Drop Copy Client -- Logged Out" );
	GetDlgItem(IDC_BUTTON_LOGON)->EnableWindow(TRUE);
}

void CFixClientDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_Log.close();
}

FIX42::Message CFixClientDlg::CreateNewOrder()
{
	FIX42::NewOrderSingle order;
	order.set( FIX::ClOrdID("1") );
	order.set( FIX::Account("0551 USER0001 WAG USER0001") );
	order.set( FIX::Symbol("SUNW") );
	order.set( FIX::OrderQty(1) );
	order.set( FIX::OrdType( '2' ) ); // limit
	order.set( FIX::Price(0.25) );
	order.set( FIX::Side( '1' ) ); // buy
	order.set( FIX::TimeInForce( '0' ) );
	order.set( FIX::SecurityType( "OPT" ) );
	order.set( FIX::MaturityMonthYear( "200401" ) );
	order.set( FIX::PutOrCall( 1 ) ); // call
	order.set( FIX::StrikePrice(10) );
	order.set( FIX::OpenClose( 'O' ) );
	return order;
}

void CFixClientDlg::OnButtonBuy() 
{
	// TODO: Add your control notification handler code here
	FIX42::Message msg = CreateNewOrder();
	msg.getHeader().setField( FIX::SenderCompID("JOEL") );
	msg.getHeader().setField( FIX::TargetCompID("RON") );
	FIX::Session::sendToTarget(msg);
}
