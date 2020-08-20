// BrassView.cpp : implementation of the CBrassView class
//

#include "stdafx.h"
#include "Brass.h"
#include "BrassDoc.h"
#include "BrassView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrassView

IMPLEMENT_DYNCREATE(CBrassView, CFormView)

BEGIN_MESSAGE_MAP(CBrassView, CFormView)
	//{{AFX_MSG_MAP(CBrassView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrassView construction/destruction

CBrassView::CBrassView()
	: CFormView(CBrassView::IDD)
{
	//{{AFX_DATA_INIT(CBrassView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
}

CBrassView::~CBrassView()
{
}

void CBrassView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrassView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CBrassView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBrassView diagnostics

#ifdef _DEBUG
void CBrassView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBrassView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBrassDoc* CBrassView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBrassDoc)));
	return (CBrassDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrassView message handlers

void CBrassView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	CFormView::OnClose();
}

void CBrassView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CBrassView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CBrassView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect ListRect;
	
	CFormView::OnSize(nType, cx, cy);
	
	if (m_List.m_hWnd > 0)
	{	
		m_List.GetWindowRect(&ListRect);
		ScreenToClient(&ListRect);
		ListRect.bottom = cy-3;
		ListRect.right = cx-3;
		m_List.MoveWindow(ListRect, TRUE);
	}
}

void CBrassView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CBrassView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);	
}
