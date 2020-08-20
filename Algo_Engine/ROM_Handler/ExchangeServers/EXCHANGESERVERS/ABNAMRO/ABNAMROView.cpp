// ABNAMROView.cpp : implementation of the CABNAMROView class
//

#include "stdafx.h"
#include "ABNAMRO.h"

#include "ABNAMRODoc.h"
#include "ABNAMROView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CABNAMROView

IMPLEMENT_DYNCREATE(CABNAMROView, CFormView)

BEGIN_MESSAGE_MAP(CABNAMROView, CFormView)
	//{{AFX_MSG_MAP(CABNAMROView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CABNAMROView construction/destruction

CABNAMROView::CABNAMROView(): CExchangeView(CABNAMROView::IDD)
{
	// TODO: add construction code here

}

CABNAMROView::~CABNAMROView()
{
}
void CABNAMROView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CABNAMROView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CABNAMROView drawing

void CABNAMROView::OnDraw(CDC* pDC)
{
	CABNAMRODoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CABNAMROView diagnostics

#ifdef _DEBUG
void CABNAMROView::AssertValid() const
{
	CView::AssertValid();
}

void CABNAMROView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CABNAMRODoc* CABNAMROView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CABNAMRODoc)));
	return (CABNAMRODoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CABNAMROView message handlers

void CABNAMROView::OnDestroy() 
{
	CExchangeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CABNAMROView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CExchangeView::OnTimer(nIDEvent);
}

void CABNAMROView::OnSize(UINT nType, int cx, int cy) 
{
	CExchangeView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
