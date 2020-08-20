/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MessageCracker.cpp : Defines the class behaviors for the application. 
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 
 *
 *******************************************************************************/

#include "stdafx.h"
#include "OMMessageCracker.h"
#include "Fields.h"
#include "MainFrm.h"
#include "CrackerDoc.h"
#include "CrackerView.h"

#include <sstream>
#include <iomanip>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrackerApp

BEGIN_MESSAGE_MAP(CCrackerApp, CWinApp)
	//{{AFX_MSG_MAP(CCrackerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
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
// CCrackerApp construction

CCrackerApp::CCrackerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCrackerApp object

CCrackerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCrackerApp initialization
bool CCrackerApp::LoadOmSpec()
{
	m_SendFields.insert( tOmFieldMap::value_type(CSV_COMMAND,"CSV_COMMAND") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TIME,"CSV_TIME") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TRADER,"CSV_TRADER") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TAG,"CSV_TAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SIDE,"CSV_SIDE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SYMBOL,"CSV_SYMBOL") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SHARES,"CSV_SHARES") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_PRICE,"CSV_PRICE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TYPE,"CSV_TYPE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TIF,"CSV_TIF") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CAPACITY,"CSV_CAPACITY") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CLEARINGID,"CSV_CLEARINGID") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CLEARINGACCOUNT,"CSV_CLEARINGACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXCHANGE,"CSV_EXCHANGE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_STATUS,"CSV_STATUS") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_INSTRUCTIONS,"CSV_INSTRUCTIONS") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_PRICECHECK,"CSV_PRICECHECK") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_STAFFORD_TAG,"CSV_STAFFORDTAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_MINQTY,"CSV_MINQTY") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_STOPPRICE,"CSV_STOPPRICE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_DISCRETIONARY,"CSV_DISCRETIONARY") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SOLICITED,"CSV_SOLICITED") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_LOCALACCOUNT,"CSV_LOCALACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_FIRM,"CSV_FIRM") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TRADE_FOR,"CSV_TRADE_FOR") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_QUERY_TAG,"CSV_QUERY_TAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_DBSYMBOL,"CSV_DBSYMBOL") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_OWNER,"CSV_OWNER") ); // 27
	m_SendFields.insert( tOmFieldMap::value_type(CSV_MULTIPLIER,"CSV_MULTIPLIER") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXDEST,"CSV_EXDEST") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXPDATE,"CSV_EXPDATE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CALLPUT,"CSV_CALLPUT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_STRIKEPRICE,"CSV_STRIKEPRICE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_UNDERLYING,"CSV_UNDERLYING") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_FLOOR,"CSV_FLOOR") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SECURITY_TYPE,"CSV_SECURITY_TYPE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_MM_ACCOUNT,"CSV_MM_ACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_MM_EXCHANGE,"CSV_MM_EXCHANGE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_OPENCLOSE,"CSV_OPENCLOSE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_TRADING_SESSION,"CSV_TRADING_SESSION") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_GIVEUPACCOUNT,"CSV_GIVEUPACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_ORDERDEST,"CSV_ORDERDEST") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_ACCOUNT,"CSV_ACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CMTAACCOUNT,"CSV_CMTAACCOUNT") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_GTD_DATE,"CSV_GTD_DATE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CUM_SHARES,"CSV_CUM_SHARES") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_LEAVE_SHARES,"CSV_LEAVE_SHARES") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXEC_TIME,"CSV_EXEC_TIME") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_AVG_PRICE,"CSV_AVG_PRICE") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_OM_TIME,"CSV_OM_TIME") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CLIENT_TIME,"CSV_CLIENT_TIME") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXEC_TAG,"CSV_EXEC_TAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_DBSYMBOL_NEW,"CSV_DBSYMBOL_NEW") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_CURRENCY,"CSV_CURRENCY") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_EXEC_INSTRUCTION,"CSV_EXEC_INSTRUCTION") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_SHORT_LENDER,"CSV_SHORT_LENDER") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_ACTTAG,"CSV_ACTION_TAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_NEW_TAG,"CSV_NEW_TAG") );
	m_SendFields.insert( tOmFieldMap::value_type(CSV_ORDER_STATUS,"CSV_ORDER_STATUS") );

	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_COMMAND,"CSV_COMMAND") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TIME,"CSV_TIME") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TRADER,"CSV_TRADER") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TAG,"CSV_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_SIDE,"CSV_SIDE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_SYMBOL,"CSV_SYMBOL") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_SHARES,"CSV_SHARES") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_PRICE,"CSV_PRICE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TYPE,"CSV_TYPE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TIF,"CSV_TIF") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CAPACITY,"CSV_CAPACITY") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CLEARINGID,"CSV_CLEARINGID") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CLEARINGACCOUNT,"CSV_CLEARINGACCOUNT") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXCHANGE,"CSV_EXCHANGE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_STATUS,"CSV_STATUS") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_ORDER_TAG,"CSV_ORDER_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXCHANGE_TAG,"CSV_EXCHANGE_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_STAFFORD_TAG,"CSV_STAFFORD_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CANCEL_TAG,"CSV_CANCEL_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXEC_CONFIRM,"CSV_EXEC_CONFIRM") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_REFERENCE,"CSV_REFERENCE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_REPORT,"CSV_REPORT") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CONTRA,"CSV_CONTRA") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_TEXT,"CSV_TEXT") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_OWNER,"CSV_OWNER") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_OMEXECUTION_TAG,"CSV_OMEXECUTION_TAG") );
//	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXEC_SHARES,"CSV_EXEC_SHARES") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_LAST_SHARES,"CSV_LAST_SHARES") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_ORIGINAL_SHARES,"CSV_ORIGINAL_SHARES") );
//	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXEC_PRICE,"CSV_EXEC_PRICE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_LAST_PRICE,"CSV_LAST_PRICE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_OWNERCANCEL,"CSV_OWNERCANCEL") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_ACCOUNT,"CSV_ACCOUNT") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CMTAACCOUNT,"CSV_CMTAACCOUNT") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_GTD_DATE,"CSV_GTD_DATE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CUM_SHARES,"CSV_CUM_SHARES") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_LEAVE_SHARES,"CSV_LEAVE_SHARES") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXEC_TIME,"CSV_EXEC_TIME") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_AVG_PRICE,"CSV_AVG_PRICE") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_OM_TIME,"CSV_OM_TIME") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CLIENT_TIME,"CSV_CLIENT_TIME") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_EXEC_TAG,"CSV_EXEC_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_DBSYMBOL_NEW,"CSV_DBSYMBOL_NEW") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_CURRENCY,"CSV_CURRENCY") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_ACTTAG,"CSV_ACTION_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_NEW_TAG,"CSV_NEW_TAG") );
	m_ReceiveFields.insert( tOmFieldMap::value_type(CSV_ORDER_STATUS,"CSV_ORDER_STATUS") );
	return true;
}

bool CCrackerApp::LoadFixSpec()
{
	HRSRC hResInfo = FindResource( AfxGetResourceHandle(), "FIX42", "FIX" );
	if( !hResInfo )
		return false;
	HANDLE hRes;
	hRes = LoadResource( AfxGetResourceHandle(), hResInfo );
	if( !hRes )
		return false;
	LPSTR lpRes = (LPSTR)LockResource( hRes );
	if( !lpRes )
		return false;
	std::stringstream file(lpRes);
	std::stringstream stm;
	while( !file.fail() )
	{
		char buff[2048];
		file.getline( buff, sizeof(buff) );
		std::string msg = buff;
		if( msg.find("<M>") != -1 )
			stm.str("");
		if( msg.find( "<Me>" ) != -1 )
			AddMessage( stm.str() );
		stm << msg;				
	}
	FreeResource( hRes );
	return true;
}


void CCrackerApp::AddMessage( const std::string & szMsg )
{
	static int i = 0;
	FixMessage msg(szMsg);
	if( i == 0 )
	{
		m_Header = msg;
	}
	else if( i == 1 )
	{
		m_Trailer = msg;
	}
	else
	{
		m_Messages.insert( tMessageMap::value_type( msg.getType(), msg ) );
	}
	i++;
}

BOOL CCrackerApp::InitInstance()
{
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

//	if( !LoadOptionSymbols() )
//		AfxMessageBox("Load Option Symbol Failed");

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

	m_pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCrackerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCrackerView));
	AddDocTemplate(m_pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	if( !LoadFixSpec() )
		return FALSE;
	if( !LoadOmSpec() )
		return FALSE;
	return TRUE;
}

FixTag CCrackerApp::FindTag( int nTagID, FixMessage & msg )
{
	try
	{
		return m_Header.FindTag(nTagID);
	}
	catch(...)
	{
		try
		{
			return m_Trailer.FindTag(nTagID);
		}
		catch(...)
		{
			try
			{
				return msg.FindTag(nTagID);
			}
			catch(...)
			{
				TRACE("Tag not defined, must be a custom tag ???\n");
				return FixTag(); // Custom Tag
			}
		}
	}
}

CDocument* CCrackerApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	return CWinApp::OpenDocumentFile(lpszFileName);
}

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
		// No message handlers
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

// App command to run the dialog
void CCrackerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCrackerApp message handlers


BOOL CCrackerApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		CCrackerView * pView = (CCrackerView *)pMain->GetActiveView();
		pView->Search();
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

bool CCrackerApp::LoadOptionSymbols()
{
	return true;

}