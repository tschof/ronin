// oatsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "oats.h"
#include "oatsDlg.h"
#include "Logger.h"

#include "ThreadPoolManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

//using namespace rivas;

COatsDlg *theDlg;
COatsCallback COatsDlg::m_OatsCallback;

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
// COatsDlg dialog

COatsDlg::COatsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COatsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COatsDlg)
	m_szCount = _T("0");
	m_szQueued = _T("0");
	//m_bConnected = FALSE;
	m_szAdminCount = _T("0");
	m_szRomCount = _T("0");
	m_Begin = 0;
	m_End = 0;
	m_szFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	theDlg = this;
	m_bPause = false;
	m_LastHB = CTime::GetCurrentTime();
}

void COatsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COatsDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_STATIC_COUNT, m_szCount);
	DDX_Text(pDX, IDC_STATIC_QUEUE, m_szQueued);
	//DDX_Check(pDX, IDC_CHECK_OM, m_bConnected);
	DDX_Text(pDX, IDC_STATIC_OMADMIN, m_szAdminCount);
	DDX_Text(pDX, IDC_STATIC_ROMCOUNT, m_szRomCount);
	DDX_DateTimeCtrl(pDX, IDC_BEGIN, m_Begin);
	DDX_DateTimeCtrl(pDX, IDC_END, m_End);
	DDX_Text(pDX, IDC_STATIC_FILE, m_szFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COatsDlg, CDialog)
	//{{AFX_MSG_MAP(COatsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, OnButtonProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COatsDlg message handlers

BOOL COatsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	m_brGreen.CreateSolidBrush( RGB(0,255,0) );
	m_brRed.CreateSolidBrush( RGB(255,0,0) );

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

	CTime t = CTime::GetCurrentTime();
	m_Begin = CTime( t.GetYear(), t.GetMonth(), t.GetDay(), 7, 0, 0 ); // 10:15PM March 19, 1999
	m_End = CTime( t.GetYear(), t.GetMonth(), t.GetDay(), 18, 0, 0 ); // 10:15PM March 19, 1999
	UpdateData( FALSE );
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_OatsCallback.StartServer();

	SetTimer( 1, 5000, NULL );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COatsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COatsDlg::OnPaint() 
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
HCURSOR COatsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL COatsDlg::DestroyWindow() 
{
	return CDialog::DestroyWindow();
}

void COatsDlg::OnButtonPause() 
{
	// TODO: Add your control notification handler code here
	COatsApp * pApp = (COatsApp *)AfxGetApp();
	if( !m_bPause )
	{
		GetDlgItem( IDC_BUTTON_PAUSE )->SetWindowText( "Resume DB" );
		m_OatsCallback.PauseDB();
		pApp->SendMail( "Database Processing Paused" );
	}
	else
	{
		GetDlgItem( IDC_BUTTON_PAUSE )->SetWindowText( "Pause DB" );
		m_OatsCallback.ResumeDB();
		pApp->SendMail( "Database Processing Resumed" );
	}
	m_bPause = !m_bPause;
	Invalidate();
	UpdateWindow();
}

HBRUSH COatsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	if( pWnd->GetDlgCtrlID() == IDC_STATE )
	{
		pDC->SetBkMode( TRANSPARENT );
		if( !m_bPause )
		{
			pWnd->SetWindowText( "Database Threads Working..." );
			return m_brGreen;
		}
		else
		{
			pWnd->SetWindowText( "Database Threads Paused" );
			return m_brRed;
		}
	}
	return hbr;
}

void COatsDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if( AfxMessageBox( "Are you sure?", MB_YESNO, 0 ) == IDNO )
		return;
	KillTimer(1);
	CDialog::OnClose();
}



void COatsDlg::OnButtonUpdate() 
{
	// TODO: Add your control notification handler code here
	UpdateStatus();
}

void COatsDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateStatus();	
	CDialog::OnTimer(nIDEvent);
}

void COatsDlg::UpdateStatus()
{
	UpdateData();
	CTime t = CTime::GetCurrentTime();
	if( t - m_LastHB > 20 )
	{
		m_OatsCallback.m_OMServer.HeartBeat();
		m_LastHB = t;
	}
	//m_bConnected = m_OatsCallback.m_OMServer.IsConnected();
	m_szRomCount.Format("%d", m_OatsCallback.m_OMServer.OMCount());
	m_szCount.Format( "%d", m_OatsCallback.GetReceived() );
	m_szQueued.Format( "%d", m_OatsCallback.GetDatabased() );
//	m_szAdminCount.Format( "%d", m_OatsCallback.GetAdminCount() );
	UpdateData( FALSE );
}

static char BASED_CODE szFilter[] = "Oats Message Files (*.log)|*.log|All Files (*.*)|*.*||";

void COatsDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg( TRUE, "log", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter );
	if( dlg.DoModal() != IDOK )
		return;
	m_szFile = dlg.GetPathName();
	UpdateData( FALSE );
}

bool COatsDlg::Parse( std::vector<std::string> & l, const char * pszMsg, char c )
{
	const char * ptr = pszMsg;
	std::string msg = "";
	while( *ptr != 0 )
	{
		if( *ptr == c )
		{
			l.push_back( msg );
			msg = "";
		}
		else
		{
			msg += *ptr;
		}
		ptr++;
	}
	l.push_back( msg );
	return true;
}

bool COatsDlg::IsValidMessage(const CString & szTime )
{
	std::vector<std::string> l;
	if( !Parse( l, szTime, ':' ) )
		return false;
	if( l.size() != 3 )
		return false;
	int nHour = atoi( l[0].c_str() );
	int nMinute = atoi( l[1].c_str() );
	int nSecond = atoi( l[2].c_str() );
	int nTime = nHour*3600+nMinute*60+nSecond;
	if( nTime < m_Begin.GetHour()*3600+m_Begin.GetMinute()*60+m_Begin.GetSecond() 
		|| nTime > m_End.GetHour()*3600+m_End.GetMinute()*60+m_End.GetSecond() )
		return false;
	return true;
}

void COatsDlg::OnButtonProcess() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if( m_szFile.IsEmpty() )
	{
		AfxMessageBox("No file selected" );
		return;
	}
	try
	{
		// Will need to handle oats csv file process if db connection problem
		// caused missing oats records in rom db.
		CStdioFile file(m_szFile,CFile::modeRead);
		DWORD dwSize = file.GetLength();
		int nTotalLine = dwSize/400;
		int nLine = 0;
		m_Progress.SetRange32( 0, nTotalLine );
		CString szMsg = "";
		int len = 0;

		while( file.ReadString( szMsg ) )
		{
			len = szMsg.GetLength();
			if (len != 0 && szMsg[len - 1] != '\n')
			{
				szMsg += "\n";
			}
			nLine++;
			m_Progress.SetPos( nLine );
			
			CString szTime;

			if (szMsg[0] == 'S' ||
				szMsg[0] == 'E')
			{
				szTime = szMsg.Mid(11, 8);
			}
			else if (szMsg[0] == 'L')
			{
				continue;
			}
			else
			{
				int nIndex = szMsg.Find( " " );
				if( nIndex == -1 )
					continue;
				//CString szDate = szMsg.Left( nIndex );
				szMsg = szMsg.Mid( nIndex+1 );
				nIndex = szMsg.Find( " " );
				if( nIndex == -1 )
					continue;
				szTime = szMsg.Left( nIndex );
				szMsg = szMsg.Mid( nIndex+1 );
			}

			//if( !IsValidMessage( szDate, szTime ) )
		//	if( !IsValidMessage(szTime ) )
		//		continue;
			m_OatsCallback.onData( szMsg );
			UpdateStatus();
		}
		file.Close();
	}
	catch(...)
	{
		AfxMessageBox( "Unknown Exception");
		return;
	}
}
