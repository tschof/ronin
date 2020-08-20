// TerraNovaView.cpp : implementation of the CArcaView class
//

#include "stdafx.h"
#include "TerraNova.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArcaView

IMPLEMENT_DYNCREATE(CArcaView, CFormView)

BEGIN_MESSAGE_MAP(CArcaView, CFormView)
	//{{AFX_MSG_MAP(CArcaView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaView construction/destruction

CArcaView::CArcaView()
	: CFormView(CArcaView::IDD)
{
	//{{AFX_DATA_INIT(CArcaView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
}

CArcaView::~CArcaView()
{
}

void CArcaView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArcaView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CArcaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CArcaView diagnostics

#ifdef _DEBUG
void CArcaView::AssertValid() const
{
	CFormView::AssertValid();
}

void CArcaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CArcaDoc* CArcaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CArcaDoc)));
	return (CArcaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArcaView message handlers

void CArcaView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	CFormView::OnClose();
}

void CArcaView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CArcaView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CArcaView::OnSize(UINT nType, int cx, int cy) 
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

void CArcaView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CArcaView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);	
}
