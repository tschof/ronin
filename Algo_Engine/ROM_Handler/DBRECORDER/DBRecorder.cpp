/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * DBRecorder.cpp : Defines the class behaviors for the application.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 09/28/05    joel        Change DB default values
 *
 *******************************************************************************/

#include "stdafx.h"
#include "DBRecorder.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "RecorderDoc.h"
#include "RecorderView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "Order.h"

#include "Process.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
HANDLE g_KillEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );

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
// CRecorderApp

BEGIN_MESSAGE_MAP(CRecorderApp, CWinApp)
	//{{AFX_MSG_MAP(CRecorderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_THREAD_MESSAGE(TH_COMMAND_ALREADY_PING, OnGotDictionary)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp construction

CRecorderApp::CRecorderApp()
{
	m_UseODBC = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecorderApp object

CRecorderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp initialization

int AFXAPI AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	ASSERT(hPrevInstance == NULL);

	int nReturnCode = -1;
	CWinThread* pThread = AfxGetThread();
	CWinApp* pApp = AfxGetApp();

	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	if (pApp != NULL && !pApp->InitApplication())
		goto InitFailure;

	// Perform specific initializations
	if (!pThread->InitInstance())
	{
		if (pThread->m_pMainWnd != NULL)
		{
			TRACE0("Warning: Destroying non-NULL m_pMainWnd\n");
			pThread->m_pMainWnd->DestroyWindow();
		}
		nReturnCode = pThread->ExitInstance();
		goto InitFailure;
	}
#ifdef NDEBUG
	try
	{
		nReturnCode = pThread->Run();
	}
	catch(...)
	{
		pThread->ExitInstance();

		system("C:\\OrderManager\\DBCrash.bat");

		nReturnCode = 0;
	}
#else
	return pThread->Run();
#endif

InitFailure:
#ifdef _DEBUG
	// Check for missing AfxLockTempMap calls
	if (AfxGetModuleThreadState()->m_nTempMapLock != 0)
	{
		TRACE1("Warning: Temp map lock count non-zero (%ld).\n",
			AfxGetModuleThreadState()->m_nTempMapLock);
	}
	AfxLockTempMaps();
	AfxUnlockTempMaps(-1);
#endif

	AfxWinTerm();
	return nReturnCode;
}

BOOL CRecorderApp::InitInstance()
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
		IDR_DBRECOTYPE,
		RUNTIME_CLASS(CRecorderDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRecorderView));
	AddDocTemplate(pDocTemplate);

	CMultiDocTemplate* pTrafficDocTemplate;
	pTrafficDocTemplate = new CMultiDocTemplate(
		IDR_TRAFFICTYPE,
		RUNTIME_CLASS(CTrafficDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
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

	m_DatFile = "c:\\Ordermanager\\Recorder.dat";
	GetPrivateProfileString("Log File Information", "Log File Path", "c:\\ordermanager\\", m_Path.GetBuffer(128), 128, DatFile()); m_Path.ReleaseBuffer();
	m_Name		= "Recorder";

	// create client window
	CChildFrame		*pNewFrame;
	CRecorderDoc	*pDoc;
	POSITION			pos;

	pDoc = (CRecorderDoc *)pDocTemplate->CreateNewDocument();
	pNewFrame = (CChildFrame *)(pDocTemplate->CreateNewFrame(pDoc, NULL));
	pNewFrame->SetTitle("Client Connections");
	pNewFrame->Restore();
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		
	if ((pos = pDoc->GetFirstViewPosition()))
	{
  		m_pClientView = (CRecorderView *)pDoc->GetNextView(pos);
		m_Clients.SetList(&m_pClientView->m_List);
	}
	
	// create error, status, and traffic windows
	CTrafficDoc		*pTrafficDoc;

	pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
	pNewFrame = (CChildFrame *)(pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
	pNewFrame->SetTitle("Traffic");
	pNewFrame->Restore();
	pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
	if ((pos = pTrafficDoc->GetFirstViewPosition()))
  		m_pTrafficView = (CTrafficView *)pTrafficDoc->GetNextView(pos);

	pTrafficDoc = (CTrafficDoc *)pTrafficDocTemplate->CreateNewDocument();
	pNewFrame = (CChildFrame *)(pTrafficDocTemplate->CreateNewFrame(pTrafficDoc, NULL));
	pNewFrame->SetTitle("Status Messages");
	pTrafficDocTemplate->InitialUpdateFrame(pNewFrame, pTrafficDoc);
	pNewFrame->Restore();
	if ((pos = pTrafficDoc->GetFirstViewPosition()))
  		m_pStatusView = (CTrafficView *)pTrafficDoc->GetNextView(pos);
		
	CFileException fe;
	if (!(m_Log.Open(m_Path + m_Name + CTime::GetCurrentTime().Format("%Y%m%d") + ".LOG", CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone, &fe)))
	{
	}
	else
		m_Log.SeekToEnd();

	COrder::Init();	

	// create the database connections
	char workString[128];

	m_pTradeDb			= new CCommonDatabase(DatFile());	// create database
	m_pTradeTable		= new CTradeTable();

	GetPrivateProfileString("Trade Table Information", "User", "rom_user", workString, 128, DatFile()); 
	m_pTradeDb->SetUser(workString);
	m_pTradeTable->SetUser(workString);
			
	GetPrivateProfileString("Trade Table Information", "Password", "resu_mor", workString, 128, DatFile()); 
	m_pTradeDb->SetPassword(workString);
	m_pTradeTable->SetPassword(workString);

	GetPrivateProfileString("Trade Table Information", "Database", "rom", workString, 128, DatFile()); 
	m_pTradeDb->SetDatabase(workString);
	m_pTradeTable->SetDatabase(workString);

	GetPrivateProfileString("Trade Table Information", "DataSource", "RON_SYB_DEV1", workString, 128, DatFile());
	m_pTradeDb->SetDataSource(workString);
					
	GetPrivateProfileString("Trade Table Information", "Server", "RON_SYB_DEV1", workString, 128, DatFile());
	m_pTradeDb->SetServer(workString);
	m_pTradeTable->SetServer(workString);

	if (m_pTradeDb->Connect())
		ProcessStatus("DB", "Succesfully Connected TradeDB to " + m_pTradeDb->Server());
	else
		ProcessStatus("DB FAILURE", "Failed To Connect TradeDB to " + m_pTradeDb->Server());

	m_pTradeTable->SetAppName(m_Name);

	try 
	{ 
		if (m_pTradeTable->Connect()) 
		{
			bConnected = TRUE;
			ProcessStatus("DB", "Successfully Connected To " + m_pTradeTable->Server());
		}
	}

	catch (char *Message)
	{
		ProcessStatus("DB", Message);
	}

	catch (CString Message)
	{
		ProcessStatus("DB", Message);
	}

	m_UseODBC = GetPrivateProfileInt("Database Information", "Use ODBC", 1,	DatFile()); 
	
	m_pTRADES = new CTrades();

	m_pRecordingThread =
		AfxBeginThread(RecordingProc, this, THREAD_PRIORITY_NORMAL ,0, 0);
	
	m_pServer	= new CServer();				// create server
	m_pServer->StartListening();				// start listening for client connections

	m_pTrades = new CTradeTableSet(m_pTradeDb->Database());

	// The main window has been initialized, so show and update it.
	pMainFrame->Restore();
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// start rv thread

	char Server[128];
	char Port[128];
	char RVNetwork[128];

	GetPrivateProfileString("Rv Information", "Server",	"",	Server,	128, DatFile());
	GetPrivateProfileString("Rv Information", "Port",		"",	Port,		128, DatFile());
	GetPrivateProfileString("Rv Information", "Broadcast Subject",		"",	m_BroadcastSubject.GetBuffer(128),		128, DatFile()); m_BroadcastSubject.ReleaseBuffer();
	GetPrivateProfileString("Rv Information", "XCAL Broadcast Subject",	"",	m_XcalBroadcastSubject.GetBuffer(128),	128, DatFile()); m_XcalBroadcastSubject.ReleaseBuffer();
	GetPrivateProfileString("Rv Information", "Network", "", RVNetwork, 128, DatFile());

	char *pRVNetwork = NULL;
	if (strcmp(RVNetwork, "") != 0) 
	{
		pRVNetwork = RVNetwork;
	}

	m_pRvHandler = new CRvOrderHandler(m_pMainWnd->m_hWnd, this, Port, pRVNetwork, Server);

	CRvOrderHandler::m_hHandlerIsDown = CreateEvent(NULL, FALSE, FALSE, NULL);
	CRvDictionaryCallback::s_ReadDictionary = true;
	m_pRvHandler->CreateThread();


	UpdateRunDate();

	/*CString s = "S,072852,RU58,-2147400883,1,NQ,1,1237.000000,2,,,,17243858,6,1,56042,,CHIHEAT03-20020531-39,,,,,RU58,83500,RU58,,WAG,,CHIHEAT03-20020531-7,,200206,,,,,";
	CMessage msg(0, s);
	StoreMessage(&msg, NULL);*/

	return TRUE;
}

int CRecorderApp::ExitInstance() 
{
	//HANDLE hThread = m_pRecordingThread->m_hThread;

	m_pStatusView = NULL;
	m_pTrafficView = NULL;

	//g_KillEvent.SetEvent();
	//WaitForSingleObject(hThread, 30000);

	if( m_pRecordingThread )
	{
		HANDLE hThread = m_pRecordingThread->m_hThread;
		SetEvent( g_KillEvent );
		WaitForSingleObject(hThread, 30000);
	}
	CloseHandle( g_KillEvent );

	if (m_pTrades != NULL)
	{
		delete m_pTrades;
	}

	if (m_pTradeDb != NULL)
	{
		delete m_pTradeDb;
	}

	if (m_pTradeTable != NULL)
	{
		delete m_pTradeTable;
	}

	if (m_pTRADES != NULL)
	{
		delete m_pTRADES;
	}

	return CMessageApp::ExitInstance();
}

// App command to run the dialog
void CRecorderApp::OnAppAbout()
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
// CRecorderApp commands

void CRecorderApp::ProcessStatus(const CString& Name, const CString& Message, char Append)
{
	ProcessLogEntry(Name, Message, Append);
		if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CRecorderApp::ProcessTraffic(const CString& Name, const CString& Traffic, char Append)
{
	ProcessLogEntry(Name, Traffic, Append);

	if (m_pTrafficView) m_pTrafficView->Add(Name + '\t' + Traffic);
}

void CRecorderApp::ProcessLogEntry(const CString& Name, const CString& Entry, char Append)
{
	if (m_Log.m_pStream)
	{
		m_Log.WriteString(CTime::GetCurrentTime().Format("%H%M%S") + "," + Name + "," + Entry + "," + '\n');
		m_Log.Flush();
	}
}

////////////////////////////////////////////////////////////////////////////////////
// message processing

void CRecorderApp::StoreMessage(CMessage* pMessage, CBaseConnection *pC)
{
	//MessageQueue()->AddTail(new CThreadObject(pMessage, pC));
	if (pMessage->Type() == CMessage::Status ||
		pMessage->Type() == CMessage::TradeReport)
	{
		m_pTRADES->AddTrade(pMessage->Message());
	}
	else if (pMessage->Type() == CMessage::Logon)
	{
		ProcessClientLogon(*pC, pMessage->Message());
	}
	else if (pMessage->Type() == CMessage::Logout)
	{
		ProcessClientLogout(*pC, pMessage->Message());
	}
}

BOOL CRecorderApp::ProcessClientLogon(CBaseConnection& pC, CCsv& CSV)
{
	CLogin Login;

	pC.m_Name = CSV.Get(CSV_TRADER);

	pC.SetLogonStatus(TRUE);

	Clients().SetClient(&pC, pC.m_Name);

	Login.SetTrader(m_Name);
	pC.SendMessage(Login.Message());

	return TRUE;
}


BOOL CRecorderApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
	pC.SendMessage(Logout.Message());

	pC.Close();

	pC.SetLogonStatus(FALSE);

	Clients().Update();

	return TRUE;
}




int CRecorderApp::AddTrade(TRADE* pTrade)
{
	int Retries = 0;
	int	Status	= -1;
	int rStatus = -1;

	// add to trade table	
	rStatus = CRecorderApp::DB_DISCONNECT;

	while ((rStatus <= 0) && (Retries < 2))
	{
		/*if (m_UseODBC)
		{
			try	{ rStatus = m_pTrades->AddTrade(CSV, *m_pPrimaryOrders); }

			catch (CDBException *e)
			{
				CString Error;
				Error.Format("EXCEPTION: Error Adding To Order Table (%s)", e->m_strError);
				ProcessStatus("Status", Error);
				e->Delete();
				if (m_pTrades->IsOpen())
					m_pTrades->Close();
				delete m_pTrades;
				m_pTrades = new CTradeTableSet(m_pTradeDb->Database());
			}
		}
		else
		{*/
		int exeStatus = 0;
		try	
		{ 
			if (pTrade->m_bOM == true)
			{
				rStatus = m_pTradeTable->AddOMTrade(pTrade, /**m_pPrimaryOrders,*/ exeStatus);
			}
			else 
			{
				rStatus = m_pTradeTable->AddNONOMTrade(pTrade, /**m_pPrimaryOrders,*/ exeStatus);
			}
		}
		catch (CString Message)
		{
			ProcessStatus("DB", Message);

			if (exeStatus == 0) // Could be a connection lost
			{
				reConnect();
			}
		}
		//}
		

		if (rStatus < 0)
		{
			Retries++;
		}
	}

	if (Retries >= 2)
	{
		CString Error, Msg;
		BuildTradeMessage(Msg, pTrade);
		Error.Format("FAILED (%d): %s", rStatus, Msg);
		ProcessStatus("Recorder", Error);

		if (rStatus != CRecorderApp::DB_DISCONNECT)
		{
			PublishStatus(rStatus, "Failed", pTrade);
		}

		if (bConnected == TRUE && rStatus < 0)
		{
			CString FailureMsg;
			// Send failure report once for the database disconnection
			if (rStatus == CRecorderApp::DB_DISCONNECT)
			{
				bConnected = FALSE;
				CString st = "Lost DB server connection";
				SendFailureReport(st);
			}
			else
			{
				CString ParmString;
				BuildParmString1(pTrade, ParmString);
				FormatFailureMsg(ParmString, rStatus, FailureMsg);
				SendFailureReport(FailureMsg);
			}
		}
	}
	else
	{
		CString Error, Msg;
		BuildTradeMessage(Msg, pTrade);
		Error.Format("Success (%d): %s", rStatus, Msg);
		ProcessTraffic("Recorder", Error);
		PublishStatus(rStatus, "Success", pTrade);
		bConnected = TRUE;
	}

	return rStatus;
}


void CRecorderApp::OnGotDictionary(WPARAM wParam, LPARAM lParam)
{
	if(wParam == TH_DICTIONARY)
		RvHandler().SetHandlerStatus(TRUE);
}
	
void CRecorderApp::PublishStatus(long TradeId, const CString& Error, const TRADE* pTrade)
{
	if (&RvHandler())
	{
		if (RvHandler().IsUp())
		{
			TibMsg	aTibMsg;
			CString	TmpString;
			CPrice	TmpPrice;

			/*aTibMsg.Append("CURRENT_STAMP",		(char *)(LPCTSTR)CSV.Get(CSV_TIME));
			aTibMsg.Append("TRADER",			(char *)(LPCTSTR)CSV.Get(CSV_TRADER));
			aTibMsg.Append("TAG",				(char *)(LPCTSTR)CSV.Get(CSV_TAG));
			aTibMsg.Append("SIDE",				Tib_i32(CSV.nGet(CSV_SIDE)));
			aTibMsg.Append("SYMBOL",			(char *)(LPCTSTR)CSV.Get(CSV_SYMBOL));
			aTibMsg.Append("EXEC_SHARES",		Tib_i32(CSV.nGet(CSV_SHARES)));
			TmpPrice = CSV.dGet(CSV_PRICE);
			aTibMsg.Append("EXEC_PRICE",		Tib_f64(TmpPrice.dPrice()));
			aTibMsg.Append("EXEC_PRICE_STR",	(char *)(LPCTSTR)TmpPrice.Price());
			aTibMsg.Append("CLEARINGACCOUNT",	(char *)(LPCTSTR)CSV.Get(CSV_CLEARINGACCOUNT));
			aTibMsg.Append("EXCHANGE",			Tib_i32(CSV.nGet(CSV_EXCHANGE)));
			aTibMsg.Append("TRADE_FOR",			(char *)(LPCTSTR)CSV.Get(CSV_TRADE_FOR));
			aTibMsg.Append("ORDER_TAG",			(char *)(LPCTSTR)CSV.Get(CSV_ORDER_TAG));
			aTibMsg.Append("EXCHANGE_TAG",		(char *)(LPCTSTR)CSV.Get(CSV_EXCHANGE_TAG));
			aTibMsg.Append("STAFFORD_TAG",		(char *)(LPCTSTR)CSV.Get(CSV_STAFFORD_TAG));
			aTibMsg.Append("CANCEL_TAG",		(char *)(LPCTSTR)CSV.Get(CSV_CANCEL_TAG));
			aTibMsg.Append("EXEC_TAG",			(char *)(LPCTSTR)CSV.Get(CSV_EXEC_TAG));
			aTibMsg.Append("EXEC_CONFIRM",		(char *)(LPCTSTR)CSV.Get(CSV_EXEC_CONFIRM));
			aTibMsg.Append("REFERENCE",			(char *)(LPCTSTR)CSV.Get(CSV_REFERENCE));
			aTibMsg.Append("REPORT",			(char *)(LPCTSTR)CSV.Get(CSV_REPORT));
			aTibMsg.Append("CONTRA",			(char *)(LPCTSTR)CSV.Get(CSV_CONTRA));
			aTibMsg.Append("TEXT",				(char *)(LPCTSTR)CSV.Get(CSV_TEXT));
			aTibMsg.Append("OWNER",				(char *)(LPCTSTR)CSV.Get(CSV_OWNER));
			aTibMsg.Append("TRADE_ID",			Tib_i32(TradeId));
			aTibMsg.Append("OMEXECUTION_TAG",	(char *)(LPCTSTR)CSV.Get(CSV_OMEXECUTION_TAG));
			aTibMsg.Append("MESSAGE",			(char *)(LPCTSTR)Error);*/

			aTibMsg.Append("TRADE_ID",			Tib_i32(TradeId));
			aTibMsg.Append("EXEC_SHARES",		Tib_i32(pTrade->m_Quantity));
			TmpPrice = pTrade->m_Price;
			aTibMsg.Append("EXEC_PRICE",		Tib_f64(TmpPrice.dPrice()));
			aTibMsg.Append("STAFFORD_TAG",		(char *)pTrade->m_StaffordTag);
			aTibMsg.Append("OMEXECUTION_TAG",	(char *)pTrade->m_OMTag);

			if (strcmp(pTrade->m_Contra, "FTP XCAL O") != 0)
			{
				RvHandler().Publish(&aTibMsg, m_BroadcastSubject);
			}
			else
			{
				RvHandler().Publish(&aTibMsg, m_XcalBroadcastSubject);
			}
			//RvHandler().Publish(&aTibMsg, m_BroadcastSubject);

			//if(CSV.nGet(CSV_EXCHANGE) == COrder::XCAL)
			//	RvHandler().Publish(&aTibMsg, m_XcalBroadcastSubject);
		}
	}
}

void CRecorderApp::reConnect()
{
	for (int i = 1; i <= 10; i++)
	{
		ProcessStatus("DB", "Starting reconnecting");
		(m_pTradeTable->pDb())->reConnect();

		if ((m_pTradeTable->pDb())->status() == DBO_SUCCESS)
		{
			break;
		}

		Sleep(2000);
	}
}

void CRecorderApp::BuildParmString1(TRADE* pTrade, CString &ParmString)
{
	CTime	CurrentTime = CTime::GetCurrentTime();
	CString CurrentTimeString = CurrentTime.Format("%Y%m%d %H:%M:%S");

	ParmString.Format("add_trade_ex_V2<trader=%s,local_ac=%s,clearing_ac=%s,firm=%s,contra=%s,base_cusip=%s,type=%s,quant=%d,price=%f,time=%s,timeofentry=%s,exchang=%s,contra_trader_acr=%s,OMTag=%s,status=%s,expdate=%s,cp=%s,strike=%f>\n\n",
					  pTrade->m_Trader,//Exec.Get(CSV_TRADE_FOR),
					  pTrade->m_LocalAC, //Exec.Get(CSV_LOCALACCOUNT),
					  pTrade->m_ClearingAC, //Exec.Get(CSV_CLEARINGACCOUNT),
					  pTrade->m_Firm, //Exec.Get(CSV_DBSYMBOL),
					  pTrade->m_Contra, //Contra.GetBuffer(Contra.GetLength()),
					  pTrade->m_BaseCusip, //Symbol,
					  pTrade->m_Type,
					  pTrade->m_Quantity, //Shares,
					  pTrade->m_Price, //Exec.dGet(CSV_PRICE),
					  pTrade->m_Time, //CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),
					  CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),
					  pTrade->m_Exchange, //
					  pTrade->m_ContraTraderAcr, //Exec.Get(CSV_CONTRA),
					  pTrade->m_OMTag, //Exec.Get(CSV_OMEXECUTION_TAG),
					  "",
					  pTrade->m_ExpDate, //ExpDate,
					  pTrade->m_CallPut, //CP,
					  pTrade->m_StrikePrice);

	CString proc;
	proc.Format("Command to run the stored procudure add_trade_ex_V2:\n\n add_trade_ex_V2 '%s','%s','%s','%s','%s','%s','%s','%s',%f,'%s',%d,%f,'%s','%s',%d,%d,%d,'%s','%s','%s',%s,%s,%s,%s,%d,'%s','%s',%s,'%s'",
				 pTrade->m_Trader,//Exec.Get(CSV_TRADE_FOR),
				 pTrade->m_LocalAC,//Exec.Get(CSV_LOCALACCOUNT),
				 pTrade->m_ClearingAC,//Exec.Get(CSV_CLEARINGACCOUNT),
				 pTrade->m_Firm,//Exec.Get(CSV_DBSYMBOL),
				 pTrade->m_Contra,//Contra.GetBuffer(Contra.GetLength()),
				 pTrade->m_BaseCusip,//Symbol,
				 pTrade->m_Type,//Type,
				 pTrade->m_ExpDate,//ExpDate,
				 pTrade->m_StrikePrice,//Strike,
				 pTrade->m_CallPut,//CP,
				 pTrade->m_Quantity,//Shares,
				 pTrade->m_Price,//Exec.dGet(CSV_PRICE),
				 pTrade->m_Time, //CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),
				 CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),//EntryTimeString.GetBuffer(EntryTimeString.GetLength()),
				 1,
				 0,
				 0,
				 "DA",
				 "NONE",
				 pTrade->m_Exchange, //Exchange,
				 "NULL",
				 "NULL",
				 "NULL",
				 "NULL",
				 0,
				 " ",
				 pTrade->m_ContraTraderAcr, //Exec.Get(CSV_CONTRA),
				 "NULL",
				 pTrade->m_OMTag //Exec.Get(CSV_OMEXECUTION_TAG)
				);

	ParmString += proc;
}

void CRecorderApp::BuildParmString2(TRADE* pTrade, CString &ParmString)
{
	CTime		CurrentTime = CTime::GetCurrentTime();
	CString CurrentTimeString = CurrentTime.Format("%Y%m%d %H:%M:%S");

	ParmString.Format("add_trade_ex_V2<trader=%s,local_ac=%s,clearing_ac=%s,firm=%s,contra=%s,base_cusip=%s,type=%s,quant=%d,price=%f,time=%s,timeofentry=%s,exchang=%s,contra_trader_acr=%s,OMTag=%s,status=%s>\n\n",
					  pTrade->m_Trader,//Exec.Get(CSV_TRADER),
					  pTrade->m_LocalAC,//Exec.Get(CSV_LOCALACCOUNT),
					  pTrade->m_ClearingAC,//Exec.Get(CSV_CLEARINGACCOUNT),
					  pTrade->m_Firm,//Exec.Get(CSV_FIRM),
					  pTrade->m_Contra,//Contra.GetBuffer(Contra.GetLength()),
					  pTrade->m_BaseCusip,//Symbol,
					  pTrade->m_Type,//"E",
					  pTrade->m_Quantity,//Shares,
					  pTrade->m_Price,//Exec.dGet(CSV_PRICE),
					  pTrade->m_Time,//CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),
					  CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),//EntryTimeString.GetBuffer(EntryTimeString.GetLength()),
					  "0",
					  pTrade->m_ContraTraderAcr,//Exec.Get(CSV_CONTRA),
					  "",
					  "");
	CString proc;

	proc.Format("Command to run the stored procudure add_trade_ex_V2:\n\n add_trade_ex_V2 '%s','%s','%s','%s','%s','%s','%s',%s,%f,%s,%d,%f,'%s','%s',%d,%d,%d,'%s','%s',%s,%s,%s,%s,%s,%d,'%s','%s',%s,%s",
				 pTrade->m_Trader,//Exec.Get(CSV_TRADER),
				 pTrade->m_LocalAC,//Exec.Get(CSV_LOCALACCOUNT),
				 pTrade->m_ClearingAC,//Exec.Get(CSV_CLEARINGACCOUNT),
				 pTrade->m_Firm,//Exec.Get(CSV_FIRM),
				 pTrade->m_Contra,//Contra.GetBuffer(Contra.GetLength()),
				 pTrade->m_BaseCusip,//Symbol,
				 pTrade->m_Type,//"E",
				 "NULL",
				 0.0,
				 "NULL",
				 pTrade->m_Quantity,//Shares,
				 pTrade->m_Price,//Exec.dGet(CSV_PRICE),
				 pTrade->m_Time,//CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),
				 CurrentTimeString.GetBuffer(CurrentTimeString.GetLength()),//EntryTimeString.GetBuffer(EntryTimeString.GetLength()),
				 1,
				 0,
				 0,
				 "DA",
				 "NONE",
				 "NULL",
				 "NULL",
				 "NULL",
				 "NULL",
				 "NULL",
				 0,
				 " ",
				 pTrade->m_ContraTraderAcr,//Exec.Get(CSV_CONTRA),
				 "NULL",
				 "NULL"
				);

	ParmString += proc;
}

void CRecorderApp::FormatFailureMsg(CString &ParmString, int ErrorCode, CString &MsgOut)
{
	CString sError;

	switch(ErrorCode)
	{
	case -1:
		sError = "symbol not in database";
		break;
	case -2:
		sError = "expiration date null/blank for future OR expiration date, call/put or strike null/blank for option";
		break;
	case -3:
		sError = "base_cusip null/blank OR your procedure was chosen as deadlock victim";
		break;
	case -5:
		sError = "uid blank -or- no entry in cleak table for this trader_acr/login combination";
		break;
	case -6:
		sError = "quantity null/zero";
		break;
	case -7:
		sError = "trader_acr, local_ac_acr, clearing_ac_id or firm_acr null/blank";
		break;
	case -10:
		sError = "combination fo trader/local/clearing accounts not valid";
		break;
	case -11:
		sError = "trader_acr invalid or inactive";
		break;
	case -12:
		sError = "time of trade more than 7 days ago (not allowed)";
		break;
	case -13:
		sError = "entry exists in userAccessRestrict for this local account";
		break;
	case -100:
		sError = "Database connection is losted";
	}

	MsgOut.Format("%s(return code = %d): %s", sError, ErrorCode, ParmString);
}

void CRecorderApp::SendFailureReport(CString& Msg)
{
	CString l_MailRecipients = "";
	CString l_MailServer;

	//CString FileName = m_Path + m_Name + ".DAT";
	GetPrivateProfileString("DB Recording Failure Notice", "Mail Recipients", "", l_MailRecipients.GetBuffer(128), 128, DatFile());
	GetPrivateProfileString("DB Recording Failure Notice", "Mail Server", "smkd-chi-et3b01.ronin.roncap.com", l_MailServer.GetBuffer(128), 128, DatFile());

	if (l_MailRecipients == "")
	{
		return;
	}

	CMailMessage l_MailMsg;
	CSMTP l_Smtp((LPCTSTR)l_MailServer);

	char l_Hostname[50];
	gethostname(l_Hostname,50);
	
	CString l_Subject = "DB Recording Failure";

	l_MailMsg.m_sFrom = l_Hostname;
	l_MailMsg.AddMultipleRecipients((LPCTSTR)l_MailRecipients);
	l_MailMsg.m_sSubject = l_Subject;
	l_MailMsg.m_sBody = Msg;

	l_Smtp.Connect();
	l_Smtp.SendMessage( &l_MailMsg );
	l_Smtp.Disconnect(); 
}


void CRecorderApp::BuildTradeMessage(CString& Msg, TRADE* pTrade)
{
	Msg.Format("%s %d %s at %f StaffordTag<%s> OMExecutionTag<%s>", 
			   COrder::SideString(pTrade->m_Side), 
			   pTrade->m_Quantity, 
			   pTrade->m_BaseCusip, 
			   pTrade->m_Price,
			   pTrade->m_StaffordTag,
			   pTrade->m_OMTag);
}

UINT RecordingProc(LPVOID pParam)
{
	TRADE Trade;
	int rc;

	while(1)
	{
		if (::WaitForSingleObject(g_KillEvent, 0) == WAIT_OBJECT_0)
		{
			return -1;
		}

		while (theApp.Trades()->GetTrade(Trade) == true)
		{
			rc = theApp.AddTrade(&Trade);
			if (rc == CRecorderApp::DB_DISCONNECT ||
				rc == DBO_DEADLOCK ||
				rc == DBO_CONNECT_LOST)
			{
				theApp.Trades()->AddTrade(Trade);
			}

			if (::WaitForSingleObject(g_KillEvent, 0) == WAIT_OBJECT_0)
			{
				return -1;
			}
		}

		Sleep(100);
	}

	return 1;
}