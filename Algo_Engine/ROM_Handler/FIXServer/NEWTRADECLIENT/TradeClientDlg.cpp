// TradeClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TradeClient.h"
#include "TradeClientDlg.h"

#include <string>
#include "FileStore.h"
#include "SocketInitiator.h"
#include "SessionSettings.h"

#include <sstream>
#include <iomanip>
#include <time.h>

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
// CTradeClientDlg dialog

CTradeClientDlg::CTradeClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTradeClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTradeClientDlg)
	m_szSymbol = _T("IBM");
	m_nQty = 100;
	m_dPrice = 10.0;
	m_nDest = 44;
	m_szAccount = _T("JOEL 0551 USER0001 WAG USER0001");
	m_szExpire = _T("200405");
	m_fStrike = 70.0f;
	m_szStatus = _T("");
	m_fStopPrice = 9.0f;
	m_nRate = 10;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hKill = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hKilled = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	m_bTimer = false;
}

void CTradeClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTradeClientDlg)
	DDX_Control(pDX, IDC_COMBO_EXECINST, m_ExecInst);
	DDX_Control(pDX, IDC_LIST_REPORT, m_lstReport);
	DDX_Control(pDX, IDC_COMBO_SECTYPE, m_SecType);
	DDX_Control(pDX, IDC_COMBO_CALLPUT, m_CallPut);
	DDX_Control(pDX, IDC_COMBO_OPENCLOSE, m_OpenClose);
	DDX_Control(pDX, IDC_LIST_ORDER, m_lstOrder);
	DDX_Control(pDX, IDC_COMBO_TPYE, m_Type);
	DDX_Control(pDX, IDC_COMBO_TIF, m_TIF);
	DDX_Control(pDX, IDC_COMBO_SIDE, m_Side);
	DDX_Text(pDX, IDC_EDIT_SYMBOL, m_szSymbol);
	DDX_Text(pDX, IDC_EDIT_QTY, m_nQty);
	DDX_Text(pDX, IDC_EDIT_PRICE, m_dPrice);
	DDX_Text(pDX, IDC_EDIT_DEST, m_nDest);
	DDX_Text(pDX, IDC_EDIT_ACCT, m_szAccount);
	DDX_Text(pDX, IDC_EDIT_EXPIRE, m_szExpire);
	DDX_Text(pDX, IDC_EDIT_STRIKE, m_fStrike);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_szStatus);
	DDX_Text(pDX, IDC_EDIT_STOP_PRICE, m_fStopPrice);
	DDX_Text(pDX, IDC_EDIT_RATE, m_nRate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTradeClientDlg, CDialog)
	//{{AFX_MSG_MAP(CTradeClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SUBMIT, OnButtonSubmit)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_BURST_ORDER, OnButtonBurstOrder)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_REPORT, OnExecutionReport )
	ON_MESSAGE( WM_REJECT, OnCancelReject )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTradeClientDlg message handlers

BOOL CTradeClientDlg::OnInitDialog()
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
	m_Side.SetCurSel(0);
	m_Type.SetCurSel(1);
	m_TIF.SetCurSel(0);
	m_SecType.SetCurSel(1);
	m_CallPut.SetCurSel(0);
	m_OpenClose.SetCurSel(0);
	m_ExecInst.SetCurSel(0);
	
	char szTemp[128];
	char szFileName[256];
	GetModuleFileName( NULL, szFileName, sizeof(szFileName) );
	char * ptr = strrchr( szFileName, '\\' );
	*ptr = 0;
	strcat( szFileName, "\\FixClient.cfg" );
	GetPrivateProfileString( "SESSION", "SenderCompID", "N/A", szTemp, sizeof(szTemp), szFileName );
	m_szSenderCompID = szTemp;
	GetPrivateProfileString( "SESSION", "TargetCompID", "N/A", szTemp, sizeof(szTemp), szFileName );
	m_szTargetCompID = szTemp;

	m_pDC = m_lstReport.GetWindowDC();
	m_nWidth = 0;

	UpdateData(FALSE);
	InitListCtrl();
	Start();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTradeClientDlg::InitListCtrl()
{
	m_lstOrder.SetExtendedStyle
	   (m_lstOrder.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	VERIFY(m_font.CreateFont(
	   14,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_NORMAL,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   "Arial"));                 // lpszFacename
	SetFont( &m_font );
	BuildColumns();
}

void CTradeClientDlg::BuildColumns()
{
	LV_COLUMN lvColumn[NUMFIELDS + 1];

	lvColumn[ORDERID].pszText	= "Client Order ID                     ";
	lvColumn[SYMBOL].pszText	= "Symbol   ";
	lvColumn[STATUS].pszText	= "Status               ";
	lvColumn[QTY].pszText		= "Qty      ";
	lvColumn[PRICE].pszText		= "Price    ";
	lvColumn[LEAVE].pszText		= "LeaveQty ";
	lvColumn[CUM].pszText		= "CumQty   ";
	lvColumn[TYPE].pszText		= "Type     ";
	lvColumn[TIF].pszText		= "TIF ";

	for (int i = 0; i < NUMFIELDS; i++)
	{
		lvColumn[i].mask = LVCF_FMT | LVCF_TEXT;
		lvColumn[i].fmt = LVCFMT_LEFT;
		m_lstOrder.InsertColumn (i, &(lvColumn[i]));
	}
	for( i = 0; i < NUMFIELDS; i++ )
		m_lstOrder.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER); //

}

void CTradeClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTradeClientDlg::OnPaint() 
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
HCURSOR CTradeClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTradeClientDlg::onCreate( const FIX::SessionID& sessionID )
{

}

void CTradeClientDlg::onLogon( const FIX::SessionID& sessionID )
{
	CString szText;
	szText.Format( "Trade Client -- Logged On (%s/%s)", m_szSenderCompID.c_str(), m_szTargetCompID.c_str() );
	SetWindowText( szText );
}

void CTradeClientDlg::onLogout( const FIX::SessionID& sessionID )
{
	SetWindowText( "Trade Client -- Logged Off" );
}

void CTradeClientDlg::toAdmin( FIX::Message& message, const FIX::SessionID& )
{

}

void CTradeClientDlg::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
{

}

void CTradeClientDlg::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
{

}

void CTradeClientDlg::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
{
	crack( message, sessionID );
}

void CTradeClientDlg::onRun()
{
	while( true )
	{
		if( ::WaitForSingleObject( m_hKill, 0 ) == WAIT_OBJECT_0 )
			break;
		Sleep(1000);
	}
	::SetEvent( m_hKilled );
}

bool CTradeClientDlg::Start()
{
	_beginthread( ThreadProc, 0, this );
	return true;
}

void CTradeClientDlg::ThreadProc( void * pParam )
{
	try
	{
		CTradeClientDlg * pDlg = (CTradeClientDlg*)pParam;
		std::string szPath = "FIXClient.cfg";
		FIX::SessionSettings settings(szPath);
		FIX::FileStoreFactory factory(".\\");

		FIX::SocketInitiator initiator( *pDlg, factory, settings );
		initiator.start();
	}
	catch(...)
	{
		return;
	}
	return;
}

void CTradeClientDlg::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	::SetEvent( m_hKill );
	::WaitForSingleObject( m_hKilled, INFINITE );
	CloseHandle( m_hKill );
	CloseHandle( m_hKilled );
	CDialog::PostNcDestroy();
}

std::string CTradeClientDlg::CreateOID()
{
	static long nSeed = 0;
	nSeed++;
	std::stringstream stm;
	time_t t = time(NULL);
	tm tt = *(localtime(&t) );
	stm << (LPCSTR)m_szSymbol 
		<< tt.tm_year+1900 
		<< std::setw(2) << std::setfill('0') << tt.tm_mon
		<< std::setw(2) << std::setfill('0') << tt.tm_mday
		<< std::setw(2) << std::setfill('0') << tt.tm_hour
		<< std::setw(2) << std::setfill('0') << tt.tm_min
		<< std::setw(2) << std::setfill('0') << tt.tm_sec
		<< std::setw(6) << std::setfill('0') << nSeed;
	return stm.str();
}


void CTradeClientDlg::onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID )
{
	FIX::Locker locker(m_mutex);
	FIX42::ExecutionReport r( report );
	m_Reports.push_back( r );
	::PostMessage( this->m_hWnd, WM_REPORT, 0, 0 );
	return;
}

void CTradeClientDlg::onMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID )
{
	FIX::Locker locker(m_mutex);
	FIX42::OrderCancelReject r( reject );
	m_Rejects.push_back( r );
	::PostMessage( this->m_hWnd, WM_REJECT, 0, 0 );
	return;
}

void CTradeClientDlg::OnButtonSubmit() 
{
	// TODO: Add your control notification handler code here
	SendOrder();
}

void CTradeClientDlg::InsertOrder( FIX::ClOrdID & cid, FIX42::OMOrder *pOrder )
{
	FIX::Locker locker(m_mutex);
	int count = m_lstOrder.GetItemCount();
	m_Orders.insert( std::make_pair<FIX::ClOrdID,FIX42::OMOrder *>( cid, pOrder ) );
	int nColumnCount = m_lstOrder.GetHeaderCtrl()->GetItemCount();
	
	CString strText;
	
	strText.Format( "%s", cid.getValue().c_str() );
	int nItem = m_lstOrder.InsertItem( LVIF_TEXT|LVIF_STATE, count, strText, LVIS_SELECTED, LVIS_SELECTED, 0, 0 );
	FIX::Symbol symbol;
	pOrder->get( symbol );
	m_lstOrder.SetItemText(nItem, 1, symbol.getValue().c_str() );
	FIX::OrdStatus status =	pOrder->getStatus();
	strText.Format( "%c", status.getValue() );
	m_lstOrder.SetItemText( nItem, 2, strText );
	m_lstOrder.SetItemData( nItem, (DWORD)pOrder );

}

void CTradeClientDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	for( int i = 0; i < m_lstOrder.GetItemCount(); i++ )
	{
		if( m_lstOrder.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			FIX42::OMOrder * pOrder = (FIX42::OMOrder *)m_lstOrder.GetItemData(i);
			FIX::ClOrdID aClOrdID;
			pOrder->get( aClOrdID );
			FIX::OrigClOrdID aOrigClOrdID = aClOrdID.getValue();
			FIX42::OrderCancelRequest cancel;
			cancel.set( aOrigClOrdID );
			std::string cid = aOrigClOrdID.getString();
			cancel.set( FIX::ClOrdID( cid+"-C" ) );
			FIX::Side side;
			pOrder->get( side );
			cancel.set( side );
			FIX::Symbol symbol;
			pOrder->get( symbol );
			cancel.set( symbol );
			cancel.getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
			cancel.getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );
			FIX::Session::sendToTarget( cancel );
		}

	}	

}

void CTradeClientDlg::OnButtonModify() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	for( int i = 0; i < m_lstOrder.GetItemCount(); i++ )
	{
		if( m_lstOrder.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			FIX42::OMOrder * pOrder = (FIX42::OMOrder *)m_lstOrder.GetItemData(i);
			FIX::ClOrdID aClOrdID;
			pOrder->get( aClOrdID );
			FIX::OrigClOrdID aOrigClOrdID = aClOrdID.getValue();
			FIX42::OrderCancelReplaceRequest replace;
			replace.set( aOrigClOrdID );
			std::string cid = aOrigClOrdID.getString();
			replace.set( FIX::ClOrdID( cid+"-R" ) );

			FIX::OrdType type;
			if( m_Type.GetCurSel() == 0 ) // market
				type = FIX::OrdType('1');
			else if( m_Type.GetCurSel() == 1 ) // limit
				type = FIX::OrdType('2');
			else if( m_Type.GetCurSel() == 2 ) // stop
				type = FIX::OrdType('3');
			else if( m_Type.GetCurSel() == 3 ) // stop limit
			{
				type = FIX::OrdType('4');
				pOrder->set( FIX::StopPx( m_fStopPrice ) );
			}
			else
				type = FIX::OrdType('2');

			pOrder->set( type );
			replace.set( type );

			FIX::TimeInForce tif;
			if( m_TIF.GetCurSel() == 0 )
				tif = FIX::TimeInForce('0');
			else if( m_TIF.GetCurSel() == 1 ) // GTC
				tif = FIX::TimeInForce('1');
			else
				tif = FIX::TimeInForce('6');

			pOrder->set( tif );
			replace.set( tif );

			FIX::Side side;
			pOrder->get( side );
			replace.set( side );

			FIX::Symbol symbol;
			pOrder->get( symbol );
			replace.set( symbol );

			FIX::OrderQty qty;
			pOrder->get( qty );
			if( qty.getValue() != m_nQty )
			{
				replace.set( FIX::OrderQty( m_nQty ) );
			}

			replace.set( FIX::Price( m_dPrice ) );

			replace.getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
			replace.getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );
			FIX::Session::sendToTarget( replace );
		}

	}	
}


void CTradeClientDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateData();
	if( m_nRate > 1000 )
		m_nRate = 1000;
	int nGap = 1000/m_nRate;
	for( int i = 0; i < m_nRate; i++ )
	{
		SendOrder();
		Sleep( nGap );
	}
	CDialog::OnTimer(nIDEvent);
}

void CTradeClientDlg::OnButtonBurstOrder() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if( !m_bTimer )
		SetTimer( 1, 1000, NULL );
	m_bTimer = true;
}

void CTradeClientDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	if( m_bTimer )
		KillTimer( 1 );
	m_bTimer = false;
}

void CTradeClientDlg::SendOrder()
{
	UpdateData();
	FIX42::OMOrder * pOrder = new FIX42::OMOrder();
	std::string szID = CreateOID();
	pOrder->set( FIX::ClOrdID( szID ) );
	pOrder->set( FIX::Account((LPCSTR)m_szAccount) );
	pOrder->set( FIX::Symbol((LPCSTR)m_szSymbol) );
	pOrder->set( FIX::OrderQty(m_nQty) );
	pOrder->set( FIX::Price(m_dPrice) );

	if( m_Side.GetCurSel() == 0 )
		pOrder->set( FIX::Side('1') );
	else if( m_Side.GetCurSel() == 1 )
		pOrder->set( FIX::Side('2') );
	else
	{
		pOrder->set( FIX::Side('5') );
		pOrder->setField( FIX::SSFirm("ShortFirm") );
	}

	if( m_Type.GetCurSel() == 0 ) // market
		pOrder->set( FIX::OrdType('1') );
	else if( m_Type.GetCurSel() == 1 ) // limit
		pOrder->set( FIX::OrdType('2') );
	else if( m_Type.GetCurSel() == 2 ) // stop
	{
		pOrder->set( FIX::OrdType('3') );
		pOrder->set( FIX::StopPx( m_fStopPrice ) );
	}
	else if( m_Type.GetCurSel() == 3 ) // stop limit
	{
		pOrder->set( FIX::OrdType('4') );
		pOrder->set( FIX::StopPx( m_fStopPrice ) );
	}
	else
		pOrder->set( FIX::OrdType('2') );

	if( m_ExecInst.GetCurSel() == 1 )
		pOrder->set( FIX::ExecInst( "G" ) );
	if( m_TIF.GetCurSel() == 0 )
		pOrder->set( FIX::TimeInForce('0') );
	else if( m_TIF.GetCurSel() == 1 ) // GTC
		pOrder->set( FIX::TimeInForce('1') );
	else
		pOrder->set( FIX::TimeInForce('6') );

	if( m_SecType.GetCurSel() == 0 )
		pOrder->set( FIX::SecurityType("CS") );
	else if( m_SecType.GetCurSel() == 1 )
		pOrder->set( FIX::SecurityType("OPT") );
	else if( m_SecType.GetCurSel() == 2 )
		pOrder->set( FIX::SecurityType("FUT") );


	if( m_OpenClose.GetCurSel() == 0 )
		pOrder->set( FIX::OpenClose('O') );
	else if( m_OpenClose.GetCurSel() == 1 )
		pOrder->set( FIX::OpenClose('C') );

	if( m_CallPut.GetCurSel() == 0 )
		pOrder->set( FIX::PutOrCall(1) );
	else if( m_CallPut.GetCurSel() == 1 )
		pOrder->set( FIX::PutOrCall(0) );

	if( m_SecType.GetCurSel() == 0 )
		pOrder->set( FIX::ExDestination((LPCSTR)m_szExpire) );
	else
		pOrder->set( FIX::MaturityMonthYear((LPCSTR)m_szExpire) );

	if( m_SecType.GetCurSel() == 0 )
	{
//		pOrder->set( FIX::MaxFloor(m_fStrike) );
	}
	else
		pOrder->set( FIX::StrikePrice(m_fStrike) );

	pOrder->setField( FIX::OMExchange(m_nDest) );

	pOrder->getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
	pOrder->getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );

	InsertOrder( FIX::ClOrdID(szID), pOrder );

	FIX::Session::sendToTarget( *pOrder );
}

void CTradeClientDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_lstReport.ResetContent();
	m_lstOrder.DeleteAllItems();
	CDialog::OnLButtonDblClk(nFlags, point);
}

LRESULT CTradeClientDlg::OnExecutionReport( WPARAM wParam, LPARAM lParam )
{
	FIX::Locker locker(m_mutex);
	while( m_Reports.size() != 0 )
	{
		FIX42::ExecutionReport report = m_Reports.front();
		m_Reports.pop_front();
		FIX::ExecType execType;
		report.get( execType );
		tOrders::iterator it;
		switch( execType )
		{
		case FIX::ExecType_PENDING_REPLACE: // = 'E';
		case FIX::ExecType_PENDING_CANCEL: // = '6';
		case FIX::ExecType_REPLACE: // = '5';
			{
				FIX::OrigClOrdID org_cid;
				report.get( org_cid );
				it = m_Orders.find( FIX::ClOrdID( org_cid.getValue() ) );
				if( it == m_Orders.end() )
				{
					m_szStatus = "Order not found: ";
					m_szStatus += report.getString().c_str();
					UpdateData( FALSE );
					return 0;
				}
			}
			break;
		case FIX::ExecType_NEW: // = '0';
		case FIX::ExecType_PARTIAL_FILL: // = '1';
		case FIX::ExecType_FILL: // = '2';
		case FIX::ExecType_DONE_FOR_DAY: // = '3';
		case FIX::ExecType_CANCELED: // = '4';
		case FIX::ExecType_STOPPED: // = '7';
		case FIX::ExecType_REJECTED: // = '8';
		case FIX::ExecType_SUSPENDED: // = '9';
		case FIX::ExecType_PENDING_NEW: // = 'A';
		case FIX::ExecType_CALCULATED: // = 'B';
		case FIX::ExecType_EXPIRED: // = 'C';
		case FIX::ExecType_RESTATED: // = 'D';
		default:
			{
				FIX::ClOrdID cid;
				report.get( cid );
				it = m_Orders.find( cid );
				if( it == m_Orders.end() )
				{
					m_szStatus = "Order not found: ";
					m_szStatus += report.getString().c_str();
					UpdateData( FALSE );
					return 0;
				}
			}
			break;
		}
		m_lstReport.AddString( report.getString().c_str() );
		m_nWidth = max( m_pDC->GetTextExtent( report.getString().c_str() ).cx, m_nWidth );
		m_lstReport.SetHorizontalExtent( m_nWidth );
		UpdateData( FALSE );

		FIX42::OMOrder * pOrder = (*it).second;
		FIX::OrdStatus status;
		report.get( status );
		pOrder->setStatus( status );
		for( int i = 0; i < m_lstOrder.GetItemCount(); i++ )
		{
			FIX42::OMOrder * pOrgOrder = (FIX42::OMOrder *)m_lstOrder.GetItemData(i);
			if( pOrgOrder == pOrder )
			{
				CString strText;
				strText.Format( "%c", status.getValue() );
				m_lstOrder.SetItemText( i, 2, strText );

				FIX::LastShares qty;
				if( report.isSetField( qty ) )
				{
					report.get( qty );
					strText.Format( "%d", (long)qty );
					m_lstOrder.SetItemText( i, 3, strText );
				}
				FIX::Price price;
				if( report.isSetField( price ) )
				{
					report.get( price );
					strText.Format( "%6.3f", (float)price );
					m_lstOrder.SetItemText( i, 4, strText );
				}

				FIX::LastPx lastPrice;
				if( report.isSetField( lastPrice ) )
				{
					report.get( lastPrice );
					strText.Format( "%6.3f", (float)lastPrice );
					m_lstOrder.SetItemText( i, 4, strText );
				}
				FIX::LeavesQty leaves;
				if( report.isSetField( leaves ) )
				{
					report.get( leaves );
					strText.Format( "%d", (long)leaves );
					m_lstOrder.SetItemText( i, 5, strText );
				}
				FIX::CumQty cum;
				if( report.isSetField( cum ) )
				{
					report.get( cum );
					strText.Format( "%d", (long)cum );
					m_lstOrder.SetItemText( i, 6, strText );
				}

				FIX::TimeInForce tif;
				if( report.isSetField( tif ) )
				{
					report.get( tif );
					strText.Format( "%c", (char)tif );
					m_lstOrder.SetItemText( i, 8, strText );
				}

				FIX::OrdType type;
				if( report.isSetField( type ) )
				{
					report.get( type );
					strText.Format( "%c", (char)type );
					m_lstOrder.SetItemText( i, 7, strText );
				}

				if( execType == FIX::ExecType_REPLACE )
				{
					FIX::ClOrdID cid;
					report.get( cid );
					m_Orders.erase( it );
					std::string szRID = cid.getString(); 
					m_Orders[szRID] = pOrder;
					pOrder->set( FIX::ClOrdID( szRID ) );
				}
				break;
			}
		}
	}
	return 0;
}

LRESULT CTradeClientDlg::OnCancelReject( WPARAM wParam, LPARAM lParam )
{
	FIX::Locker locker(m_mutex);
	while( m_Rejects.size() != 0 )
	{
		FIX42::OrderCancelReject reject = m_Rejects.front();
		m_Rejects.pop_front();
		m_lstReport.AddString( reject.getString().c_str() );
		m_nWidth = max( m_pDC->GetTextExtent( reject.getString().c_str() ).cx, m_nWidth );
		m_lstReport.SetHorizontalExtent( m_nWidth );
		UpdateData( FALSE );
		FIX::OrigClOrdID org_cid;
		reject.get( org_cid );
		tOrders::iterator it;
		it = m_Orders.find( FIX::ClOrdID( org_cid.getValue() ) );
		if( it == m_Orders.end() )
		{
			m_szStatus = "Order not found: ";
			m_szStatus += reject.getString().c_str();
			UpdateData( FALSE );
			return 0;
		}
		FIX42::OMOrder * pOrder = (*it).second;
		for( int i = 0; i < m_lstOrder.GetItemCount(); i++ )
		{
			FIX42::OMOrder * pOrgOrder = (FIX42::OMOrder *)m_lstOrder.GetItemData(i);
			if( pOrgOrder == pOrder )
			{
				CString strText;
				FIX::CxlRejResponseTo response;
				reject.get( response );
				if( response == '1' )
					strText.Format( "Cancel Rejected" );
				else if( response == '2' )
					strText.Format( "Replace Rejected" );
				else
					strText.Format( "Unknown Rejected" );
				m_lstOrder.SetItemText( i, 2, strText );
				break;
			}
		}
	}
	return 0;
}
