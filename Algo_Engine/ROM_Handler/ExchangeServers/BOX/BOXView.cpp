// BOXView.cpp : implementation of the CBOXView class
//

#include "stdafx.h"
#include "BOX.h"

#include "BOXDoc.h"
#include "BOXView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBOXView

IMPLEMENT_DYNCREATE(CBOXView, CFormView)

BEGIN_MESSAGE_MAP(CBOXView, CFormView)
	//{{AFX_MSG_MAP(CBOXView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBOXView construction/destruction

CBOXView::CBOXView():CExchangeView(CBOXView::IDD)
{
	// TODO: add construction code here

}

CBOXView::~CBOXView()
{
}

void CBOXView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CBOXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBOXView drawing

void CBOXView::OnDraw(CDC* pDC)
{
	CBOXDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CBOXView diagnostics

#ifdef _DEBUG
void CBOXView::AssertValid() const
{
	CView::AssertValid();
}

void CBOXView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBOXDoc* CBOXView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBOXDoc)));
	return (CBOXDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBOXView message handlers

void CBOXView::OnDestroy()
{
	CExchangeView::OnDestroy();

	// TODO: Add your message handler code here

}

void CBOXView::OnSize(UINT nType, int cx, int cy)
{
	CExchangeView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

}

void CBOXView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CExchangeView::OnTimer(nIDEvent);
}
