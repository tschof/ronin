// CBOEView.cpp : implementation of the CCBOEView class
//

#include "stdafx.h"
#include "CBOE.h"

#include "CBOEDoc.h"
#include "CBOEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCBOEView

IMPLEMENT_DYNCREATE(CCBOEView, CFormView)

BEGIN_MESSAGE_MAP(CCBOEView, CFormView)
	//{{AFX_MSG_MAP(CCBOEView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCBOEView construction/destruction

CCBOEView::CCBOEView():CExchangeView(CCBOEView::IDD)
{
	// TODO: add construction code here

}

CCBOEView::~CCBOEView()
{
}

void CCBOEView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CCBOEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCBOEView drawing

void CCBOEView::OnDraw(CDC* pDC)
{
	CCBOEDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CCBOEView diagnostics

#ifdef _DEBUG
void CCBOEView::AssertValid() const
{
	CView::AssertValid();
}

void CCBOEView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCBOEDoc* CCBOEView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCBOEDoc)));
	return (CCBOEDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCBOEView message handlers

void CCBOEView::OnDestroy() 
{
	CExchangeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CCBOEView::OnSize(UINT nType, int cx, int cy) 
{
	CExchangeView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CCBOEView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CExchangeView::OnTimer(nIDEvent);
}
