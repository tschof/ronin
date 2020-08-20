/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * CrackerView.cpp : implementation of the CCrackerView class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 09/28/05    joel        Change default TargetCompID = RON
 *
 *******************************************************************************/

#include "stdafx.h"
#include "OMMessageCracker.h"
#include "MainFrm.h"
#include "CrackerDoc.h"
#include "CrackerView.h"
#include "OmListener.h"
#include "OmSession.h"
#include "FileStore.h"
#include "SocketInitiator.h"
#include "SessionSettings.h"
#include "csv.h"

#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrackerView

char szOrder[] = "Receiving: 8=FIX.4.2.9=263.35=D.49=DEVTEST1.56=TD.34=367.52=20040413-16:03:31.1=TESTFIX 0551 12345678D9 WAG TESTLOCAL.11=00-0bd1-0002-0001-00.15=USD.21=1.38=10.40=2.44=4.20000000.47=R.54=1.55=CSCO.59=0.60=20040413-16:03:31.77=O.100=44.167=OPT.200=200404.201=1.202=20.000.203=0.204=0.205=17.10=101.";

#define NUMFIELDS			5
#define COLUMN_TAG			0
#define COLUMN_DESC			1
#define COLUMN_VALUE		2
#define COLUMN_REQUIRED		3
#define COLUMN_NOTES		4

int nColWidth[NUMFIELDS] = { 50, 150, 260, 60, 250 };


IMPLEMENT_DYNCREATE(CCrackerView, CFormView)

BEGIN_MESSAGE_MAP(CCrackerView, CFormView)
	//{{AFX_MSG_MAP(CCrackerView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_SHOWALL, OnButtonShowall)
	ON_WM_CREATE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_LOG, OnClickListLog)
	ON_WM_KEYDOWN()
	ON_NOTIFY(LVN_ODSTATECHANGED, IDC_LIST_LOG, OnOdstatechangedListLog)
//	ON_BN_CLICKED(IDC_BUTTON_BEGIN, OnButtonBegin)
//	ON_BN_CLICKED(IDC_BUTTON_AFTER, OnButtonAfter)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, OnButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_OM, OnButtonOm)
	ON_BN_CLICKED(IDC_BUTTON_FIX, OnButtonFix)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_LOG, GetDispInfo)
	ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST_LOG, OnCacheHint)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_LOG, OnColClick)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LIST_LOG, OnOdfinditem)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_LOG, OnDrawListMsg )
	// ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdraw ) for derived CListCtrl
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrackerView construction/destruction

CCrackerView::CCrackerView()
	: CFormView(CCrackerView::IDD)
{
	//{{AFX_DATA_INIT(CCrackerView)
	m_szKey = _T("");
	m_szSendKey = _T("Sending ");
	m_szRecvKey = _T("Recevied ");
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_bHasResults = false;
	m_hKill = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hKilled = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}

CCrackerView::~CCrackerView()
{
}

void CCrackerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrackerView)
	DDX_Control(pDX, IDC_LIST_RESPONSE, m_lstResponse);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_LIST_MSG, m_lstMsg);
	DDX_Control(pDX, IDC_LIST_LOG, m_lstLog);
	DDX_Text(pDX, IDC_EDIT_KEY, m_szKey);
	DDX_Text(pDX, IDC_EDIT_SEND_KEY, m_szSendKey);
	DDX_Text(pDX, IDC_EDIT_REC_KEY, m_szRecvKey);
	//}}AFX_DATA_MAP
}

BOOL CCrackerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CCrackerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	m_Progress.SetRange(0,100);
	GetParentFrame()->CenterWindow();
	InitListControl();
	m_szSenderCompID = "JOEL";
	m_szTargetCompID = "RON";
}

void CCrackerView::Reset()
{
	m_lstMsg.DeleteAllItems();
	m_lstLog.DeleteAllItems();
	m_Results.clear();
	m_bHasResults = false;
}

void CCrackerView::InitListControl()
{
	static bool bInit = false;
	if( !bInit )
	{
		m_lstLog.SetExtendedStyle(LVS_EX_FULLROWSELECT/*|LVS_SHOWSELALWAYS*/);
		m_lstMsg.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
		LVCOLUMN Col;
		Col.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
		Col.iSubItem = 0;
		Col.pszText = "Message";
		Col.cchTextMax = 4096;
		CRect rect;
		m_lstLog.GetWindowRect( &rect );
		m_nMsgWidth = rect.Width()-20;
		Col.cx = m_nMsgWidth+20;
		Col.fmt = LVCFMT_LEFT;
		m_lstLog.InsertColumn( 0, &Col );
		m_lstLog.SetBkColor( RGB( 255, 255, 255 ) );

		LV_COLUMN lvColumn[NUMFIELDS];
		lvColumn[COLUMN_TAG].pszText = "Field";
		lvColumn[COLUMN_DESC].pszText = "Description";
		lvColumn[COLUMN_VALUE].pszText = "Value";
		lvColumn[COLUMN_REQUIRED].pszText = "Required";
		lvColumn[COLUMN_NOTES].pszText = "Notes";
		for( int i = 0; i < NUMFIELDS; i++ )
		{
			lvColumn[i].mask	= LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
			lvColumn[i].fmt		= LVCFMT_LEFT;
			lvColumn[i].cx		= nColWidth[i];
			m_lstMsg.InsertColumn( i, &lvColumn[i] );
		}
		m_lstMsg.SetExtendedStyle( LVS_EX_FULLROWSELECT );
	}
	bInit = true;
}
/////////////////////////////////////////////////////////////////////////////
// CCrackerView printing

BOOL CCrackerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCrackerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCrackerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CCrackerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CCrackerView diagnostics

#ifdef _DEBUG
void CCrackerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCrackerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CCrackerDoc* CCrackerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCrackerDoc)));
	return (CCrackerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCrackerView message handlers

void CCrackerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( lHint != 0 )
		m_Progress.SetPos( lHint );
	
}

void CCrackerView::AddLogMsg() 
{
	// Fill class data from dialog.
	m_bHasResults = false;
	m_lstLog.DeleteAllItems();
	m_lstLog.SetItemCountEx(GetDocument()->m_Messages.size());
	m_lstLog.Invalidate();
}

void CCrackerView::OnCacheHint(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVCACHEHINT* pHint = (NMLVCACHEHINT*)pNMHDR;
	pHint->hdr;
	pHint->iFrom;
	pHint->iTo;
}

void CCrackerView::GetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		// then display the appropriate column
		switch(pItem->iSubItem)
		{
		case 0:
			{
				CString szMsg = GetMessage(pItem->iItem).c_str();
				CDC * pDC = m_lstLog.GetWindowDC();
				int nWidth = pDC->GetTextExtent( szMsg ).cx;
				if( nWidth > m_nMsgWidth )
				{
					m_lstLog.SetColumnWidth( 0, m_nMsgWidth );
					m_nMsgWidth = nWidth;
				}
				m_lstLog.ReleaseDC( pDC );
				if( szMsg.GetLength() > pItem->cchTextMax )
				{
					lstrcpy( pItem->pszText, szMsg );
				}
				else	
					lstrcpy( pItem->pszText, szMsg );
			}
			break;
		default:
			//ASSERT(0);
			break;
		}
	}
}

// This function is called when the column headers are clicked.
// (The event occurs on the mouseup event.)
// Sort the array by the appropriate column contents.
//
void CCrackerView::OnColClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	LVCOLUMN Col;
	CString ColName;

	// Set text only.
	Col.mask=LVCF_TEXT;

	// Reset the header columns (to indicate the sort order).
	for(int i = 0; i < 2; i++)
	{
		if( i == pNMListView->iSubItem)
		{
			ColName.LoadString(IDS_LISTCOLSEL + i);
			SortByCol(i);
		}
		else
			ColName.LoadString(IDS_LISTCOL + i);

		Col.pszText=ColName.GetBuffer(ColName.GetLength()+1);
		Col.cchTextMax=ColName.GetLength()+1;
		m_List.SetColumn(i, &Col);
		ColName.ReleaseBuffer();
	}

	// Redraw the list.
	m_List.Invalidate();

	*pResult = 0;
	*/
}

// Handle the quick key event.
//  Move to the next item starting with the pressed key.
//  Searches from the current item + 1 to the end, then starts from 0 to the current item.
//
void CCrackerView::OnOdfinditem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	NMLVFINDITEM* pFindInfo = (NMLVFINDITEM*)pNMHDR;
	LVFINDINFO FindItem = pFindInfo->lvfi;

	int i;
	if(FindItem.flags & LVFI_STRING)
	{
		TCHAR chKeyL = FindItem.psz[0];
		TCHAR chKeyH = _toupper(FindItem.psz[0]);;

		// Search to end.
		for( i = pFindInfo->iStart; i < m_LabelCount; i++ )
		{
			if(		( chKeyL == m_arLabels[i].m_strText[0] )
				||	( chKeyH == m_arLabels[i].m_strText[0] )
				)
			{
				*pResult = i;
				return;
			}
		}

		// Search from 0 to start.
		for( i = 0; i < pFindInfo->iStart; i++ )
		{
			if(		( chKeyL == m_arLabels[i].m_strText[0] )
				||	( chKeyH == m_arLabels[i].m_strText[0] )
				)
			{
				*pResult = i;
				return;
			}
		}
	}

	*pResult = -1;	// Default action.
	*/
}

void CCrackerView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
}

void CCrackerView::OnButtonSearch() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_lstLog.DeleteAllItems();
	m_lstMsg.DeleteAllItems();
	if( m_szKey.IsEmpty() )
	{
		AfxMessageBox("Please enter a search key!");
		return;
	}
	CWaitCursor wait;
	tMessageIterator it;
	if( m_bHasResults )
	{
		tMessages temp;
		for( it = m_Results.begin(); it != m_Results.end(); it++ )
		{
			if( (*it).find( (LPCSTR)m_szKey ) != -1 )
				temp.push_back( (*it) );
		}
		m_Results.clear();
		m_Results = temp;

	}
	else
	{
		m_Results.clear();
		for( it = GetDocument()->m_Messages.begin(); it != GetDocument()->m_Messages.end(); it++ )
		{
			if( (*it).find( (LPCSTR)m_szKey ) != -1 )
				m_Results.push_back( (*it) );
		}
	}

	if( m_Results.size() > 0 )
	{
		m_bHasResults = true;
	}
	m_lstLog.SetItemCountEx(m_Results.size());
	m_lstLog.Invalidate();
}

void CCrackerView::OnButtonShowall() 
{
	// TODO: Add your control notification handler code here
	AddLogMsg();
}

std::string & CCrackerView::GetMessage( int nIndex )
{
	if( m_bHasResults )
	{
		return m_Results[nIndex];
	}
	else
	{
		return GetDocument()->m_Messages[nIndex];
	}
}

void CCrackerView::OnDrawListMsg( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
    *pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
	switch( pLVCD->nmcd.dwDrawStage )
	{
	case CDDS_ITEMPREPAINT:
		TRACE( "pLVCD->nmcd.dwDrawStage CDDS_ITEMPREPAINT\n" );
		break;
	case CDDS_ITEMPOSTPAINT:
		TRACE( "pLVCD->nmcd.dwDrawStage CDDS_ITEMPOSTPAINT\n" );
		break;
	case CDDS_ITEMPREERASE:
		TRACE( "pLVCD->nmcd.dwDrawStage CDDS_ITEMPREERASE\n" );
		break;
	case CDDS_ITEMPOSTERASE:
		TRACE( "pLVCD->nmcd.dwDrawStage CDDS_ITEMPOSTERASE\n" );
		break;
	default:
		TRACE( "pLVCD->nmcd.dwDrawStage = %d\n", pLVCD->nmcd.dwDrawStage );
		break;
	}

	switch( pLVCD->nmcd.uItemState )
	{
		case CDIS_SELECTED:
			TRACE( "Item State CDIS_SELECTED\n");
			break;
		case CDIS_GRAYED:
			TRACE( "Item State CDIS_GRAYED\n");
			break;
		case CDIS_DISABLED:
			TRACE( "Item State CDIS_DISABLED\n");
			break;
		case CDIS_CHECKED:
			TRACE( "Item State CDIS_CHECKED\n");
			break;
		case CDIS_FOCUS:
			TRACE( "Item State CDIS_FOCUS\n");
			break;
		case CDIS_DEFAULT:
			TRACE( "Item State CDIS_DEFAULT\n");
			break;
		case CDIS_HOT:
			TRACE( "Item State CDIS_HOT\n");
			break;
		case CDIS_MARKED:
			TRACE( "Item State CDIS_MARKED\n");
			break;
		case CDIS_INDETERMINATE:
			TRACE( "Item State CDIS_INDETERMINATE\n");
			break;
		default:
			TRACE( "Item State %d\n", pLVCD->nmcd.uItemState );
	}

    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
    {
		*pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
    {
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.
		// We'll cycle the colors through red, green, and light blue.

		COLORREF crText;
		LVITEM rItem;
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
		ZeroMemory ( &rItem, sizeof(LVITEM) );
		rItem.mask  = LVIF_TEXT;
		char szBuf[1024];
		ZeroMemory ( szBuf, sizeof(szBuf) );
		rItem.pszText = szBuf;
		rItem.cchTextMax = sizeof(szBuf);
		rItem.iItem = nItem;
		m_lstLog.GetItem ( &rItem );

		CString szMsg = szBuf;
		if ( szMsg.Find("Sending") != -1 )
			crText = RGB(192,0,0);
		else
			crText = RGB(0,0,192);
		
		// Store the color back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = RGB( 255, 255, 255 );
		/*
        // If this item is selected, redraw the icon with its normal colors.
        CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );
		pDC->SetTextColor( RGB(0,0,255) );
		//pDC->SetBkMode( TRANSPARENT );
		pDC->SetBkColor( RGB( 100, 100, 0 ) );
        CRect rc;
        // Get the rect that holds the item's icon.
        // m_lstMsg.GetItemRect ( nItem, &rcIcon, LVIR_ICON );
        m_lstLog.GetItemRect( nItem, &rc, LVIR_LABEL );
		int nWidth = pDC->GetTextExtent( szBuf ).cx;
		pDC->PatBlt( rc.left, rc.right, rc.Width(), rc.Height(), BLACKNESS );
		pDC->DrawText( szBuf, &rc, DT_SINGLELINE|DT_BOTTOM|DT_LEFT );
        *pResult = CDRF_SKIPDEFAULT;
		*/
		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
    }
}


int CCrackerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

CString CCrackerView::GetSelected( CCrackerApp::tOmFieldMap & fields )
{
	int nIndex = m_lstLog.GetNextItem( -1, LVNI_SELECTED );
	CString szMsg = GetMessage( nIndex ).c_str();
	CCrackerApp * pApp = (CCrackerApp *)AfxGetApp();
	if( ( nIndex = szMsg.Find(m_szSendKey+"E," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szSendKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szRecvKey+"E," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szRecvKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szSendKey+"R," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szSendKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szRecvKey+"R," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szRecvKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szSendKey+"C," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szSendKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szRecvKey+"C," ) ) != -1 )
	{
		fields = pApp->m_SendFields;
		nIndex += m_szRecvKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szSendKey+"S," ) ) != -1 )
	{
		fields = pApp->m_ReceiveFields;
		nIndex += m_szSendKey.GetLength();
	}
	else if( ( nIndex = szMsg.Find(m_szRecvKey+"S," ) ) != -1 )
	{
		fields = pApp->m_ReceiveFields;
		nIndex += m_szRecvKey.GetLength();
	}
	else
		return szMsg;
	szMsg = szMsg.Mid( nIndex );
	return szMsg;
}

void CCrackerView::OnClickListLog(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	CCrackerApp::tOmFieldMap fields;
	CString szMsg = GetSelected(fields);
	if( szMsg.Find( "8=FIX" ) != -1 )
	{
		CrackFixMsg( szMsg );
		return;
	}

	m_lstMsg.DeleteAllItems();
	ASSERT( szMsg[0] == 'E' || szMsg[0] == 'R' || szMsg[0] == 'C' || szMsg[0] == 'S' );
	std::vector<std::string>	vValues;
	std::stringstream			stm;
	int nFields = 0;
	for( int i = 0; i < szMsg.GetLength(); i++ )
	{
		char c = szMsg[i];
		switch( c )
		{
		case ',':
			vValues.push_back(stm.str());
			stm.str("");
			nFields++;
			break;
		default:
			stm << c;
		}
	}
	vValues.push_back(stm.str());
	for( i = nFields-1; i >= 0; i-- )
	{
		CString szText;
		szText.Format( "%d", i+1 );
		m_lstMsg.InsertItem( 0, szText );
		if( i == 44 )
			TRACE("Does not exist!\n");
		std::string szDesc = fields[i+1];
		m_lstMsg.SetItemText( 0, COLUMN_DESC, szDesc.c_str() );
		m_lstMsg.SetItemText( 0, COLUMN_VALUE, vValues[i+1].c_str() );
	}

	*pResult = 0;
}

void CCrackerView::CrackFixMsg( CString & szMsg )
{
	m_lstMsg.DeleteAllItems();
	int nIndex = szMsg.Find( "8=" );
	if( nIndex == -1 )
		return;
	szMsg = szMsg.Mid( nIndex );
	std::vector<int>			vTags;
	std::vector<std::string>	vValues;
	std::stringstream			stm;
	for( int i = 0; i < szMsg.GetLength(); i++ )
	{
		char c = szMsg[i];
		switch( c )
		{
		case '=':
			vTags.push_back(atoi(stm.str().c_str()));
			stm.str("");
			break;
		case 0x01:
			vValues.push_back(stm.str());
			stm.str("");
			break;
		default:
			stm << c;
		}
	}
	std::string m_szTypeID;
	std::string m_szType;
	for( i = 0; i < vTags.size(); i++ )
	{
		if( vTags[i] == 35 )
		{
			m_szTypeID = vValues[i];
			break;
		}
			
	}
	CCrackerApp * pApp = (CCrackerApp *)AfxGetApp();
	CCrackerApp::tMessageMap::iterator it = pApp->m_Messages.find(m_szTypeID);
	FixMessage msg;
	if( it != pApp->m_Messages.end() )
	{
		m_szType = (*it).second.getName();
		msg = (*it).second;
//		UpdateData( FALSE );
	}
	else
	{
		AfxMessageBox("Unsupported message type!");
		return;
	}
	int nSize = vTags.size();
	for( i = vTags.size()-1; i >=0; i-- )
	{
		CString szText;
		szText.Format( "%d", vTags[i] );
		m_lstMsg.InsertItem( 0, szText );
		FixTag tag = pApp->FindTag( vTags[i], msg );
		m_lstMsg.SetItemText( 0, COLUMN_DESC, tag.getTagName().c_str() );
		m_lstMsg.SetItemText( 0, COLUMN_VALUE, vValues[i].c_str() );
		m_lstMsg.SetItemText( 0, COLUMN_REQUIRED, tag.getRequired() == true ? "Y":"N" );
		m_lstMsg.SetItemText( 0, COLUMN_NOTES, tag.getDesc().c_str() );
	}
}


void CCrackerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCrackerView::Search()
{
	OnButtonSearch();
}



void CCrackerView::OnOdstatechangedListLog(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVODSTATECHANGE* pStateChanged = (NMLVODSTATECHANGE*)pNMHDR;
	// TODO: Add your control notification handler code here
	TRACE( "OnOdstatechangedListLog\n");
	*pResult = 0;
}
/*
void CCrackerView::OnButtonBegin() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_lstLog.DeleteAllItems();
	m_lstMsg.DeleteAllItems();
	int nItem = m_lstLog.GetNextItem( -1, LVNI_SELECTED );
	if( m_szKey.IsEmpty() )
	{
		AfxMessageBox("Please enter a search key!");
		return;
	}
	CWaitCursor wait;
	tMessageIterator it;
	bool bFound = false;
	if( m_bHasResults )
	{
		tMessages temp;
		for( it = m_Results.begin(); it != m_Results.end(); it++ )
		{
			if( !bFound && (*it).find( (LPCSTR)m_szKey ) != -1 )
			{
				temp.push_back( (*it) );
				bFound = true;
			}
			else if( bFound )
				temp.push_back( (*it) );
			else
				continue;
		}
		m_Results.clear();
		m_Results = temp;

	}
	else
	{
		m_Results.clear();
		for( it = GetDocument()->m_Messages.begin(); it != GetDocument()->m_Messages.end(); it++ )
		{
			if( !bFound && (*it).find( (LPCSTR)m_szKey ) != -1 )
			{
				m_Results.push_back( (*it) );
				bFound = true;
			}
			else if( bFound )
				m_Results.push_back( (*it) );
			else
				continue;
		}
	}

	if( m_Results.size() > 0 )
	{
		m_bHasResults = true;
	}
	m_lstLog.SetItemCountEx(m_Results.size());
	m_lstLog.Invalidate();
	
}

void CCrackerView::OnButtonAfter() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int nItem = m_lstLog.GetNextItem( -1, LVNI_SELECTED );
	if( nItem == -1 )
	{
		AfxMessageBox("Nothing is selected!");
		return;
	}
	if( m_szKey.IsEmpty() )
	{
		AfxMessageBox("Please enter a search key!");
		return;
	}
	CWaitCursor wait;
	tMessageIterator it;
	bool bFound = false;
	int nSelected = nItem+1;
	
	if( m_bHasResults )
	{
		for( it = m_Results.begin()+nItem+1; it != m_Results.end(); it++ )
		{
			if( (*it).find( (LPCSTR)m_szKey ) != -1 )
			{
				bFound = true;
				m_lstLog.SetItemState( nSelected, LVIS_SELECTED, LVIS_SELECTED );
				break;
			}
			nSelected++;
		}
	}
	else
	{
		for( it = GetDocument()->m_Messages.begin()+nItem+1; it != GetDocument()->m_Messages.end(); it++ )
		{
			if( (*it).find( (LPCSTR)m_szKey ) != -1 )
			{
				bFound = true;
				m_lstLog.SetItemState( nSelected, LVIS_SELECTED, LVIS_SELECTED );
				break;
			}
			nSelected++;
		}
	}

	m_lstLog.Invalidate();
	
}

*/
void CCrackerView::OnButtonProcess() 
{
	// TODO: Add your control notification handler code here
	Start();
	return;
	tMessages	m_Messages;
	m_lstLog.DeleteAllItems();
	m_lstMsg.DeleteAllItems();
	m_Results.clear();
	tMessageIterator it;
	bool bFound = false;
	for( it = GetDocument()->m_Messages.begin(); it != GetDocument()->m_Messages.end(); it++ )
	{
		std::string szTemp = (*it);
		if( (*it).find( (LPCSTR)"Command to run the stored procudure add_trade_ex_V2:" ) != -1 )
		{
			it++;
			it++;
			szTemp = (*it);
			m_Results.push_back( (*it) );
			bFound = true;
		}
	}
	if( m_Results.size() > 0 )
	{
		m_bHasResults = true;
	}
	m_lstLog.SetItemCountEx(m_Results.size());
	m_lstLog.Invalidate();
}

void CCrackerView::OnButtonOm() 
{
	// TODO: Add your control notification handler code here
	CCrackerApp::tOmFieldMap fields;
	CString szMsg = GetSelected(fields);
	int nIndex = szMsg.Find( "8=FIX" );
	if( nIndex != -1 )
	{
		AfxMessageBox("This is a FIX Message!");
		return;
	}
	else
	{
		szMsg += '\n';
		Send( std::string((LPCSTR)szMsg) );
	}
}

void CCrackerView::OnButtonFix() 
{
	// TODO: Add your control notification handler code here
	CCrackerApp::tOmFieldMap fields;
	CString szMsg = GetSelected(fields);
	int nIndex = szMsg.Find( "8=FIX" );
	if( nIndex != -1 )
	{
		szMsg = szMsg.Mid( nIndex );
		FIX::Message msg;
		msg.setString((LPCSTR)szMsg);
		msg.getHeader().setField( FIX::BeginString("FIX.4.2") );
		msg.getHeader().setField( FIX::SenderCompID("JOEL") );
		msg.getHeader().setField( FIX::TargetCompID("RON") );
		FIX::Session::sendToTarget( msg );
	}
	else
	{
		AfxMessageBox("This is a OM Message!");
		return;
	}
}

void CCrackerView::onCreate( const FIX::SessionID& sessionID )
{

}

void CCrackerView::onLogon( const FIX::SessionID& sessionID )
{
	CString szText;
	szText.Format( "Logged On (%s/%s)", m_szSenderCompID.c_str(), m_szTargetCompID.c_str() );
	CMainFrame * pWnd = (CMainFrame *)AfxGetMainWnd();
	pWnd->SetStatus( szText );
}

void CCrackerView::onLogout( const FIX::SessionID& sessionID )
{
	CString szText;
	szText.Format( "Logged Off (%s/%s)", m_szSenderCompID.c_str(), m_szTargetCompID.c_str() );
	CMainFrame * pWnd = (CMainFrame *)AfxGetMainWnd();
	pWnd->SetStatus( szText );
}

void CCrackerView::toAdmin( FIX::Message& message, const FIX::SessionID& )
{

}

void CCrackerView::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
{

}

void CCrackerView::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
{

}

void CCrackerView::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
{
	crack( message, sessionID );
}

void CCrackerView::onRun()
{
	while( true )
	{
		if( ::WaitForSingleObject( m_hKill, 0 ) == WAIT_OBJECT_0 )
			break;
		Sleep(1000);
	}
	::SetEvent( m_hKilled );
}

bool CCrackerView::Start()
{
	m_pListener = new COmListener(this);
	m_pListener->Create(8008);
	m_pListener->Listen();
	_beginthread( ThreadProc, 0, this );
	return true;
}

void CCrackerView::ThreadProc( void * pParam )
{
	try
	{
		CCrackerView * pDlg = (CCrackerView*)pParam;
		std::string szPath = "c:\\FixServer\\FIXClient.cfg";
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

void CCrackerView::onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID )
{
	std::string szMsg = report.getString();
	m_lstResponse.AddString( szMsg.c_str() );
}

void CCrackerView::onMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID )
{
	std::string szMsg = reject.getString();
	m_lstResponse.AddString( szMsg.c_str() );
}

void CCrackerView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	::SetEvent( m_hKill );
	::WaitForSingleObject( m_hKilled, INFINITE );
	CloseHandle( m_hKill );
	CloseHandle( m_hKilled );
	
}

void CCrackerView::OnMessage( std::string & szMsg )
{
	m_lstResponse.AddString( szMsg.c_str() );
	CCsv csv;
	CString msg = szMsg.c_str();
	csv.Bust( msg );
	switch( csv.cGet( CSV_COMMAND ) )
	{
	case 'L':
		{
			std::string temp = szMsg;
			temp += "\n";
			GetSession()->Send( temp.c_str(), temp.length() );
		}
		break;
	case 'K':
		break;
	case 'S':
		break;
	case 'E':
		break;
	default:
		break;
	}

}

int CCrackerView::Send( std::string & szMsg )
{
	GetSession()->Send( szMsg.c_str(), szMsg.length() );
	return -1;
}
