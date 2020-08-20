// oats.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "oats.h"
#include "oatsDlg.h"

#include "TestDB.h"
#include "mdump.h"
#include "smtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COatsApp
MiniDumper dump("Oats");

BEGIN_MESSAGE_MAP(COatsApp, CWinApp)
	//{{AFX_MSG_MAP(COatsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_REGISTERED_THREAD_MESSAGE(TF_ADMIN_REQUEST, OnProcessAdminRequest)
	ON_REGISTERED_THREAD_MESSAGE(TF_ADMINCOUNT_UPDATE, OnProcessAdminCountUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COatsApp construction

COatsApp::COatsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COatsApp object

COatsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COatsApp initialization

BOOL COatsApp::InitInstance()
{
	AfxSocketInit();
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

#ifdef _DEBUG
//	CTestDB db( "C:\\OATS-2002-05-06.csv" );
#endif

	COatsDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK

	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int COatsApp::ExitInstance()
{
	return 0;
}

void COatsApp::SendMail( LPCSTR szMsg )
{
	CString szMailRecipients = "";
	CString szMailServer;
	GetPrivateProfileString("Settings", "Mail Recipients", "joseph.loescher@ronin-capital.com", szMailRecipients.GetBuffer(256), 256, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Settings", "Mail Server", "smkd-chi-et3b01.ronin.roncap.com", szMailServer.GetBuffer(256), 256, "\\OrderManager\\Oats.dat");
	if( szMailRecipients == "" )
		return;
	CMailMessage msg;
	CSMTP smtp( (LPCSTR)szMailServer );
	char szHost[50];
	gethostname( szHost, 50 );
	CString szSubject = "Oats New Alert";
	msg.m_sFrom = szHost;
	msg.AddMultipleRecipients( (LPCSTR)szMailRecipients );
	msg.m_sSubject = szSubject;
	msg.m_sBody = szMsg;
	smtp.Connect();
	smtp.SendMessage( &msg );
	smtp.Disconnect();
	szMailRecipients.ReleaseBuffer();
	szMailServer.ReleaseBuffer();

}


void COatsApp::OnProcessAdminRequest( UINT wParam, LONG lParam )
{
	CCsv* pCsv = (CCsv*)lParam;

	if (pCsv)
	{
		(COatsDlg::GetCallback())->ProcessAdminRequest(pCsv);
		delete pCsv;
	}

}


void COatsApp::OnProcessAdminCountUpdate( UINT wParam, LONG lParam )
{
	if (theDlg)
	{
		CString s;
		s.Format("%ld", (long)lParam);
		theDlg->SetAdminCount(s);
	}
}

