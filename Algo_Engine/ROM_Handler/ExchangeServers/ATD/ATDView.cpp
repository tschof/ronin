// ATDView.cpp : implementation of the CATDView class
//

#include "stdafx.h"
#include "ATD.h"

#include "ATDDoc.h"
#include "ATDView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CATDView

IMPLEMENT_DYNCREATE(CATDView, CFormView)

BEGIN_MESSAGE_MAP(CATDView, CFormView)
	//{{AFX_MSG_MAP(CATDView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CATDView construction/destruction

CATDView::CATDView():CExchangeView(CATDView::IDD)
{
	// TODO: add construction code here

}

CATDView::~CATDView()
{
}

void CATDView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CATDView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CATDView drawing

void CATDView::OnDraw(CDC* pDC)
{
	CATDDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CATDView diagnostics

#ifdef _DEBUG
void CATDView::AssertValid() const
{
	CView::AssertValid();
}

void CATDView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CATDDoc* CATDView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CATDDoc)));
	return (CATDDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CATDView message handlers

void CATDView::OnDestroy()
{
	CExchangeView::OnDestroy();

	// TODO: Add your message handler code here

}

void CATDView::OnSize(UINT nType, int cx, int cy)
{
	CExchangeView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

}

void CATDView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CExchangeView::OnTimer(nIDEvent);
}
