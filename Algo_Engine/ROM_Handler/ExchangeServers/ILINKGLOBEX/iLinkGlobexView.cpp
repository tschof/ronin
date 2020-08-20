//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"
#include "iLinkGlobexDoc.h"
#include "iLinkGlobexView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CILinkGlobexView, CFormView)

BEGIN_MESSAGE_MAP(CILinkGlobexView, CFormView)
	//{{AFX_MSG_MAP(CILinkGlobexView)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexView construction/destruction

CILinkGlobexView::CILinkGlobexView()
	: CFormView(CILinkGlobexView::IDD)
{
	//{{AFX_DATA_INIT(CILinkGlobexView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
}

CILinkGlobexView::~CILinkGlobexView()
{
}

void CILinkGlobexView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CILinkGlobexView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CILinkGlobexView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexView diagnostics

#ifdef _DEBUG
void CILinkGlobexView::AssertValid() const
{
	CFormView::AssertValid();
}

void CILinkGlobexView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CILinkGlobexDoc* CILinkGlobexView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CILinkGlobexDoc)));
	return (CILinkGlobexDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexView message handlers

void CILinkGlobexView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	CFormView::OnClose();
}

void CILinkGlobexView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CILinkGlobexView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CILinkGlobexView::OnSize(UINT nType, int cx, int cy) 
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

void CILinkGlobexView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CILinkGlobexView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);	
}
