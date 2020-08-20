// BRUTView.cpp : implementation of the CBRUTView class
//

#include "stdafx.h"
#include "BRUT.h"
#include "BRUTDoc.h"
#include "BRUTView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBRUTView

IMPLEMENT_DYNCREATE(CBRUTView, CFormView)

BEGIN_MESSAGE_MAP(CBRUTView, CFormView)
	//{{AFX_MSG_MAP(CBRUTView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBRUTView construction/destruction

CBRUTView::CBRUTView()
	: CFormView(CBRUTView::IDD)
{
	//{{AFX_DATA_INIT(CBRUTView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
}

CBRUTView::~CBRUTView()
{
}

void CBRUTView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBRUTView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CBRUTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBRUTView diagnostics

#ifdef _DEBUG
void CBRUTView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBRUTView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBRUTDoc* CBRUTView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBRUTDoc)));
	return (CBRUTDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBRUTView message handlers

void CBRUTView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	CFormView::OnClose();
}

void CBRUTView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CBRUTView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CBRUTView::OnSize(UINT nType, int cx, int cy) 
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

void CBRUTView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CBRUTView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);
}
