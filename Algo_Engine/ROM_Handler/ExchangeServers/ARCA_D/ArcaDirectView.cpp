// ArcaDirectView.cpp : implementation of the CArcaDirectView class
//

#include "stdafx.h"
#include "ArcaDirect.h"
#include "ArcaDirectDoc.h"
#include "ArcaDirectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectView

IMPLEMENT_DYNCREATE(CArcaDirectView, CFormView)

BEGIN_MESSAGE_MAP(CArcaDirectView, CFormView)
	//{{AFX_MSG_MAP(CArcaDirectView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectView construction/destruction

CArcaDirectView::CArcaDirectView()
	: CFormView(CArcaDirectView::IDD)
{
	//{{AFX_DATA_INIT(CArcaDirectView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
}

CArcaDirectView::~CArcaDirectView()
{
}

void CArcaDirectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArcaDirectView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CArcaDirectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectView diagnostics

#ifdef _DEBUG
void CArcaDirectView::AssertValid() const
{
	CFormView::AssertValid();
}

void CArcaDirectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CArcaDirectDoc* CArcaDirectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CArcaDirectDoc)));
	return (CArcaDirectDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectView message handlers

void CArcaDirectView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	CFormView::OnClose();
}

void CArcaDirectView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CArcaDirectView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CArcaDirectView::OnSize(UINT nType, int cx, int cy) 
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

void CArcaDirectView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CArcaDirectView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);
}
