/ TerraNovaView.cpp : implementation of the CTerraNovaView class
//

#include "stdafx.h"
#include "Actor.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView

IMPLEMENT_DYNCREATE(CTerraNovaView, CFormView)

BEGIN_MESSAGE_MAP(CTerraNovaView, CFormView)
	//{{AFX_MSG_MAP(CTerraNovaView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView construction/destruction

CTerraNovaView::CTerraNovaView()
	: CFormView(CTerraNovaView::IDD)
{
	//{{AFX_DATA_INIT(CTerraNovaView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;

}

CTerraNovaView::~CTerraNovaView()
{
}

void CTerraNovaView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTerraNovaView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CTerraNovaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView diagnostics

#ifdef _DEBUG
void CTerraNovaView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTerraNovaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTerraNovaDoc* CTerraNovaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTerraNovaDoc)));
	return (CTerraNovaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaView message handlers

void CTerraNovaView::OnClose() 
{
	if (m_TimerId >= 0)
		{
		KillTimer(m_TimerId);
		m_TimerId = -1;
		}
	
	CFormView::OnClose();
}

void CTerraNovaView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
	
	
}

void CTerraNovaView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		((CActSession *)(pApp->Exchange())).ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CTerraNovaView::OnSize(UINT nType, int cx, int cy) 
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

void CTerraNovaView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CTerraNovaView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
		CRect Rect;
		GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		OnSize(0, Rect.right, Rect.bottom);
	
}
