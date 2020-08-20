// TerraNovaView2.cpp : implementation of the CTerraNovaView2 class
//

#include "stdafx.h"
#include "Actor.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView2

IMPLEMENT_DYNCREATE(CTerraNovaView2, CFormView)

BEGIN_MESSAGE_MAP(CTerraNovaView2, CFormView)
	//{{AFX_MSG_MAP(CTerraNovaView2)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView2 construction/destruction

CTerraNovaView2::CTerraNovaView2()
	: CFormView(CTerraNovaView2::IDD)
{
	//{{AFX_DATA_INIT(CTerraNovaView2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;

}

CTerraNovaView2::~CTerraNovaView2()
{
}

void CTerraNovaView2::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerraNovaView2)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CTerraNovaView2::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView2 diagnostics

#ifdef _DEBUG
void CTerraNovaView2::AssertValid() const
{
	CFormView::AssertValid();
}

void CTerraNovaView2::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTerraNovaDoc* CTerraNovaView2::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTerraNovaDoc)));
	return (CTerraNovaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView2 message handlers

void CTerraNovaView2::OnClose() 
{
	if (m_TimerId >= 0)
		{
		KillTimer(m_TimerId);
		m_TimerId = -1;
		}
	
	CFormView::OnClose();
}

void CTerraNovaView2::OnDestroy() 
{
	if (m_TimerId >= 0)
		{
		KillTimer(m_TimerId);
		m_TimerId = -1;
		}

	CFormView::OnDestroy();
	
	
}

void CTerraNovaView2::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
		{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange()->ProcessTimer();
		}
	
	CFormView::OnTimer(nIDEvent);
}

void CTerraNovaView2::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect		ListRect;
	
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

void CTerraNovaView2::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CTerraNovaView2::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
		CRect Rect;
		GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		OnSize(0, Rect.right, Rect.bottom);
	
}
