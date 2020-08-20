//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//	CILinkGlobexApp::CILinkGlobexApp()
//	CILinkGlobexApp::InitInstance()
//	CILinkGlobexApp::CreateDocumentTemplates()
//	CILinkGlobexApp::OnAppAbout()
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ConnectionFrame.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "iLinkGlobexDoc.h"
#include "iLinkGlobexView.h"
#include "FixSession.h"
#include "SeqDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_CONNECTION_ATTEMPTS 3;

int CILinkGlobexApp::m_messageNumber = 0;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	void SetVersion(CString& version)
	{
		m_version = version;
	}

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_version;
	CString m_title;
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

	 m_title	= "About ";
	 m_version	= "unknown";
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
		CString copyRight;

		copyRight.LoadString(IDS_COPYRIGHT);

		pEdit->SetWindowText(copyRight);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexApp

BEGIN_MESSAGE_MAP(CILinkGlobexApp, CWinApp)
	//{{AFX_MSG_MAP(CILinkGlobexApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_LOGOUT, OnLogout)
	ON_COMMAND(IDM_RESET_OUTGOING, OnResetOutgoing)
	ON_COMMAND(IDM_RESET_INCOMING, OnResetIncoming)
	ON_COMMAND(IDM_CLEAR_OUTGOING, OnClearOutgoing)
	ON_COMMAND(IDM_CLEAR_INCOMING, OnClearIncoming)
	ON_COMMAND(IDM_TOGGLE_DISPLAY, OnToggleDisplay)
	ON_UPDATE_COMMAND_UI(IDM_TOGGLE_DISPLAY, OnUpdateToggleDisplay)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexApp construction

CILinkGlobexApp::CILinkGlobexApp()
{
	m_bDisplayTraffic = false;

#if defined GLOBEX_FOC
	m_AppSpecificName = "ILinkGlobex(FOC)";
#else
	m_AppSpecificName = "ILinkGlobex(ML)";
#endif

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CILinkGlobexApp object

CILinkGlobexApp theApp;


//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
BOOL CILinkGlobexApp::InitInstance()
{
	CMainFrame* pMainFrame;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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

	CreateDocumentTemplates();

	if (FALSE == CreateMainFrame(&pMainFrame))
	{
		return FALSE;
	}

	InitializeDATFilePath();

	if(!CMessageApp::InitInstance())
	{
		return FALSE;
	}

	InitializeLogFilePath();

	InitializeFailoverParameters();

	InitializeSequenceNumbers();

	InitializeAuditTrail();

    InitalizeFirmNameForAudit();

	CreateClientWindows();
	CreateExchangeWindows();
	CreateTrafficWindows();

	try 
	{
		InitializeOrderManager();
		InitializeOrderHandler();
		InitializeFixSession();
		InitializeOrderStatus();
	}
	catch(CFixSessionException&)
	{
		ProcessErrorEx(MODE_ERR_NOTIFY,"ILinkGlobex App", "Initialization of FIX session failed.");
		return FALSE;
	}

	COrder::Init();
	UpdateRunDate();

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->Restore();
	pMainFrame->UpdateWindow();

	return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
int CILinkGlobexApp::ExitInstance() 
{
	m_pErrorView = NULL;
	m_pStatusView = NULL;
	m_pOrderView = NULL;
	
	delete m_auditTrail;

	return CMessageApp::ExitInstance();
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
// App command to run the dialog
void CILinkGlobexApp::OnAppAbout()
{
	try
	{
		CAboutDlg aboutDlg;

		aboutDlg.SetVersion(GetVersion());

		aboutDlg.DoModal();
	}
	catch(std::exception& e)
	{
		CHAR szBuf[80]; 
 
		sprintf(szBuf, "Error Displaying About Info: %s\n", e.what()); 

		AfxMessageBox(szBuf);
	}
}


//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::CreateDocumentTemplates()
{
	m_pILinkGlobexDocTemplate = new CMultiDocTemplate(IDI_ILINKTYPE, RUNTIME_CLASS(CILinkGlobexDoc), RUNTIME_CLASS(CConnectionFrame),RUNTIME_CLASS(CILinkGlobexView));

	AddDocTemplate(m_pILinkGlobexDocTemplate);

	m_pTrafficDocTemplate = new CMultiDocTemplate(IDI_TRAFFICTYPE, RUNTIME_CLASS(CTrafficDoc), RUNTIME_CLASS(CConnectionFrame), RUNTIME_CLASS(CTrafficView));

	AddDocTemplate(m_pTrafficDocTemplate);
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
BOOL CILinkGlobexApp::CreateMainFrame(CMainFrame** ppMainFrame)
{
	// create main MDI Frame window
	*ppMainFrame = new CMainFrame;

	if (!(*ppMainFrame)->LoadFrame(IDR_MAINFRAME))
	{
		return FALSE;
	}

	m_pMainWnd = *ppMainFrame;

	return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::InitializeDATFilePath(void)
{
	m_Name	= AfxGetAppName();

#if defined GLOBEX_FOC
	m_DatFile = "\\Ordermanager\\" + m_Name + "FOC" + ".dat";
#else
	m_DatFile = "\\Ordermanager\\" + m_Name + ".dat";
#endif

}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::InitializeLogFilePath(void)
{
	CString Path;

	GetPrivateProfileString("Log File Information",	"Path", m_Path,	Path.GetBuffer(128), 128, DatFile()); 

	m_Path = Path.GetBuffer(0);

	Path.ReleaseBuffer();
}
//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::InitializeFailoverParameters(void)
{
	int useFailOver;

	m_usePrimaryConnection = true;

	useFailOver = GetPrivateProfileInt(m_Name + " Server", "UseFailOver", 0, DatFile());

	m_useFailOver = (useFailOver == 0) ? false : true;

	m_maxConnectionAttempts = GetPrivateProfileInt(m_Name + " Server", "ReconnectAttempts", 0, DatFile());
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::InitializeSequenceNumbers(void)
{
	int recycleDay;
	int testVar;

	TString outgoingSequenceFile;
	TString incomingSequenceFile;

	CMessageApp* pApp = (CMessageApp*) AfxGetApp();

	TString fileName = pApp->DatFile();

	outgoingSequenceFile.resize(129);
	incomingSequenceFile.resize(129);

	recycleDay = GetPrivateProfileInt("Fix Information", "SequenceRecycleDay", 0, fileName.c_str());

	testVar = GetPrivateProfileInt("Fix Information", "ForceSequenceRecycle", 0, fileName.c_str());

	m_ForceRecycle = (0 != testVar) ? true : false;

	GetPrivateProfileString("Fix Information",	"Outgoing Sequence File", "", (char*) outgoingSequenceFile.c_str(), 128, fileName.c_str());
	GetPrivateProfileString("Fix Information",	"Incoming Sequence File", "", (char*) incomingSequenceFile.c_str(), 128, fileName.c_str());

	WritePrivateProfileString("Fix Information", "ForceSequenceRecycle", "0", fileName.c_str());

	if (false == m_ForceRecycle)
	{
		if (CTime::GetCurrentTime().GetDayOfWeek() != recycleDay)
		{
			return;
		}

		if (FALSE == pApp->IsFirstRun())
		{
			return;
		}

		// This indicates that the day to recycle the
		// sequence numbers has occured.  There set the
		// the flag so that the 141=Y is sent to exchange
		// in the logon.
		m_ForceRecycle = true;
	}

	try
	{
		CFile::Remove(outgoingSequenceFile.c_str());
		CFile::Remove(incomingSequenceFile.c_str());
	}
	catch(CFileException& e)
	{
		CString error = "Unable to remove sequence file";
		error += e.m_strFileName.GetBuffer(0);

		pApp->ProcessErrorEx(M_ERR_NOTIFY_EL_WARNING, "CILinkGlobexApp::InitializeSequenceNumbers", error);
	}
	catch(...)
	{
		CString error = "Unable to remove sequence file";
		pApp->ProcessErrorEx(M_ERR_NOTIFY_EL_WARNING, "CILinkGlobexApp::InitializeSequenceNumbers", error);
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::InitializeAuditTrail(void)
{
	CString auditPath;

#if defined GLOBEX_FOC
	auditPath = "\\Ordermanager\\" + m_Name + "FOC" + "\\CMEAudit";
#else
	auditPath = "\\Ordermanager\\" + m_Name + "\\CMEAudit";
#endif
    auditPath += CTime::GetCurrentTime().FormatGmt("%Y%m%d");

	m_auditTrail = new ZLog(auditPath.GetBuffer(0));
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::CreateExchangeWindows()
{
	m_pExchangeView	= NULL;

	CreateDocumentWindow(m_pILinkGlobexDocTemplate, CString("Exchange Connection"),
        (CFormView**) &m_pExchangeView);

	if (NULL != m_pExchangeView)
	{
		m_pExchangeView->CreateTimer(15000);
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::CreateClientWindows()
{
	m_pClientView = NULL;

	CreateDocumentWindow(m_pILinkGlobexDocTemplate, CString("Client Connections"),
        (CFormView**) &m_pClientView);

	if (NULL != m_pClientView)
	{
		m_Clients.SetList(&m_pClientView->m_List);
	}

}

//**************************************************************************
// 
//	Method:
//	CILinkGlobexApp::CreateTrafficWindows()
//
//	Parameters:
//
//
//	Return:
//	void
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::CreateTrafficWindows()
{
	CreateDocumentWindow(m_pTrafficDocTemplate, CString("Order Traffic"),	(CFormView**) &m_pOrderView);
	CreateDocumentWindow(m_pTrafficDocTemplate, CString("Status Messages"),	(CFormView**) &m_pStatusView);
	CreateDocumentWindow(m_pTrafficDocTemplate, CString("Error Messages"),	(CFormView**) &m_pErrorView);
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::CreateDocumentWindow(CMultiDocTemplate* pDocTemplate,
                                           CString& title, CFormView** ppFormView)
{	
	CDocument			*pDocument;
	CConnectionFrame	*pNewFrame;
	POSITION			pos;

	pDocument	= (CDocument *)pDocTemplate->CreateNewDocument();
	pNewFrame	= (CConnectionFrame *)(pDocTemplate->CreateNewFrame(pDocument, NULL));

	pNewFrame->SetTitle(title);
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDocument);
	pNewFrame->Restore();

	if ((pos = pDocument->GetFirstViewPosition()))
	{
  		*ppFormView = (CFormView *)pDocument->GetNextView(pos);
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnClearOutgoing()
{
	((CFixSession&)Exchange()).SetNextOutgoingSeqNum(1);
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnClearIncoming()
{
	((CFixSession&)Exchange()).SetNextIncomingSeqNum(1);
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnResetOutgoing()
{
	CSeqDlg dlg;

	dlg.SetSeqNum( ((CFixSession&)Exchange()).GetNextOutgoingSeqNum());

	if(dlg.DoModal() == IDOK)
	{
		((CFixSession&)Exchange()).SetNextOutgoingSeqNum(dlg.SeqNum());
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnResetIncoming()
{
	CSeqDlg dlg;

	dlg.SetSeqNum( ((CFixSession&)Exchange()).GetNextIncomingSeqNum());

	if(dlg.DoModal() == IDOK)
	{
		((CFixSession&)Exchange()).SetNextIncomingSeqNum(dlg.SeqNum());
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnLogout() 
{
	Exchange().Logout();
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnToggleDisplay()
{
	m_bDisplayTraffic = !m_bDisplayTraffic;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::OnUpdateToggleDisplay(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bDisplayTraffic);
}

void CILinkGlobexApp::InitalizeFirmNameForAudit()
{
    CString firmName;
    CString firmId;
    CString fti;

    GetPrivateProfileString("Fix Information", "Session ID", "", firmName.GetBuffer(128),
        128, m_DatFile);
    firmName.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "Firm ID", "", firmId.GetBuffer(128),
        128, m_DatFile);
    firmId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "FTI", "", fti.GetBuffer(128),
        128, m_DatFile);
    fti.ReleaseBuffer();

    m_AuditFirm += firmName;
    m_AuditFirm += firmId;
    m_AuditFirm += fti;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//  
//  NOT Thread Safe !!
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
void CILinkGlobexApp::Audit(const CCsv& auditCSV, char *source, char *direction,
                            bool bTime, const CString& orderTag)
{
	try
	{
		CCsv writeCSV;
		CString string;
		
		int testNum;
		char testChar;
		
		CTime currentTime = CTime::GetCurrentTime();
		CTime writeTime;
		
		if(bTime)
		{
			writeTime = CTime(
							currentTime.GetYear(), 
							currentTime.GetMonth(), 
							currentTime.GetDay(),
							atol(auditCSV.Get(CSV_TIME).Left(2)),
							atol(auditCSV.Get(CSV_TIME).Mid(2,2)),
							atol(auditCSV.Get(CSV_TIME).Right(2))
							);
		}
		else
		{
			writeTime = currentTime;
		}

		writeCSV.SetAt(0, source);
		writeCSV.SetAt(1, CILinkGlobexApp::m_messageNumber++);
		writeCSV.SetAt(2, currentTime.Format("%Y-%m-%d"));
		writeCSV.SetAt(3, currentTime.Format("%H:%M:%S"));
		writeCSV.SetAt(4, "CME");
		writeCSV.SetAt(5, direction);
		
		testNum = auditCSV.nGet(CSV_STATUS);
		
		if(testNum == COrder::Rejected || testNum == COrder::CancelRejected)
		{
			writeCSV.SetAt(6, "REJECT");
		}
		else
		{
			writeCSV.SetAt(6, "OK");
		}

		writeCSV.SetAt(7, orderTag);//auditCSV.Get(CSV_STAFFORD_TAG));
		writeCSV.SetAt(8, auditCSV.Get(CSV_STAFFORD_TAG));
		testChar = auditCSV.cGet(CSV_COMMAND);
		
		switch(testChar)
		{
			case CMessage::Cancel:
				writeCSV.SetAt(9, "CANCEL");
			break;
			
			case CMessage::Order:
				writeCSV.SetAt(9, "ENTER");
			break;

			case CMessage::Replace:
				writeCSV.SetAt(9, "ENTER");
			break;

			case CMessage::Logon:
				writeCSV.SetAt(9, "LOGIN");
				string = ((CCsv&)writeCSV).Build();
				m_auditTrail->Log(((CString&)string).GetBuffer(0));
				return;
			break;
			
			case CMessage::Logout:
				writeCSV.SetAt(9, "LOGOFF");
				string = ((CCsv&)writeCSV).Build();
				m_auditTrail->Log(((CString&)string).GetBuffer(0));
				return;
			break;

			case CMessage::Status:
				switch(testNum)
				{
					case COrder::Filled:
					case COrder::PartiallyFilled:
						writeCSV.SetAt(9, "EXECUTION");
						writeCSV.SetAt(19, auditCSV.Get(CSV_PRICE));
						writeCSV.SetAt(22, auditCSV.Get(CSV_CONTRA));
					break;

					case COrder::PendingNew:
					case COrder::Open:
					case COrder::Rejected:
						writeCSV.SetAt(9, "ENTER");
					break;

					case COrder::PendingCancel:
					case COrder::Canceled:
					case COrder::CancelRejected:
						writeCSV.SetAt(9, "CANCEL");
					break;
				}
		};
		
		testNum = auditCSV.nGet(CSV_SIDE);

		if(testNum == COrder::Buy)
		{
			writeCSV.SetAt(10, "B");
		}
		else if(testNum == COrder::Sell)
		{
			writeCSV.SetAt(10, "S");
		}

		writeCSV.SetAt(11, auditCSV.Get(CSV_SHARES));
		writeCSV.SetAt(12, auditCSV.Get(CSV_SYMBOL));
		writeCSV.SetAt(13, ParseProduct(auditCSV.Get(CSV_SYMBOL)));
		writeCSV.SetAt(14, ParseDate(auditCSV.Get(CSV_SYMBOL)));
		writeCSV.SetAt(15, "");
		writeCSV.SetAt(16, "");
		writeCSV.SetAt(18, "");
		testNum = auditCSV.nGet(CSV_TYPE);
		
		switch(testNum)
		{
		case COrder::Market:
			writeCSV.SetAt(20, "MKT");
			break;
		case COrder::Limit:
			writeCSV.SetAt(20, "LMT");
			if(!writeCSV.Get(19).GetLength())
				writeCSV.SetAt(17, auditCSV.Get(CSV_PRICE));
			break;
        case COrder::Stop:
            writeCSV.SetAt(20, "STP");
            break;
        case COrder::StopLimit:
            writeCSV.SetAt(20, "STL");
            break;
        case COrder::MarketLimit:
            writeCSV.SetAt(20, "MKT");
            break;
		};
		
		writeCSV.SetAt(21, "DAY");
		writeCSV.SetAt(23, auditCSV.Get(CSV_TRADER));
		writeCSV.SetAt(24, 4);
		writeCSV.SetAt(25, 1);
		writeCSV.SetAt(26, auditCSV.Get(CSV_CLEARINGACCOUNT));
		writeCSV.SetAt(27, "");
		writeCSV.SetAt(28, 'N');
		writeCSV.SetAt(29, "");
		writeCSV.SetAt(30, auditCSV.Get(CSV_TEXT));
		writeCSV.SetAt(31, m_AuditFirm);//auditCSV.Get(CSV_INSTRUCTIONS));
		
		string = ((CCsv&)writeCSV).Build();
		m_auditTrail->Log(((CString&)string).GetBuffer(0));
	}
	catch(...)
	{
	}
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
CString CILinkGlobexApp::ParseProduct(const CString& Symbol)
{
	if(!Symbol.GetLength())
	{
		return "";
	}

	return Symbol.Left(Symbol.GetLength() - 2);
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
CString CILinkGlobexApp::ParseDate(const CString& Symbol)
{
	char buffer[10];
	CString strCurrentYearFirst = CTime::GetCurrentTime().Format("%Y");
	CString strCurrentYearLast = strCurrentYearFirst.Right(1);
	strCurrentYearFirst = strCurrentYearFirst.Left(3);
	
	if(!Symbol.GetLength()) return "";
	CString Date = Symbol.Right(2);
	char Month = Date.GetAt(0);
	
	int FullExpYear = atol(strCurrentYearFirst + Date.GetAt(1));
	int FullYear = CTime::GetCurrentTime().GetYear();
	
	if(FullExpYear < FullYear)
	{
		FullExpYear += 10;
	}

	switch(Month)
	{
	case 'F':
		Date = "01";
		break;
	case 'G':
		Date = "02";
		break;
	case 'H':
		Date = "03";
		break;
	case 'J':
		Date = "04";
		break;
	case 'K':
		Date = "05";
		break;
	case 'M':
		Date = "06";
		break;
	case 'N':
		Date = "07";
		break;
	case 'Q':
		Date = "08";
		break;
	case 'U':
		Date = "09";
		break;
	case 'V':
		Date = "10";
		break;
	case 'X':
		Date = "11";
		break;
	case 'Z':
		Date = "12";
	}

	Date = CString(ltoa(FullExpYear, buffer, 10)) + "-" + Date;
	return Date;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Return:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************
bool CILinkGlobexApp::ForceRecycle()
{
	return m_ForceRecycle;
}
