// ISEView.cpp : implementation of the CISEView class
//

#include "stdafx.h"
#include "ISE.h"

#include "ISEDoc.h"
#include "ISEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISEView

IMPLEMENT_DYNCREATE(CISEView, CFormView)

BEGIN_MESSAGE_MAP(CISEView, CFormView)
	//{{AFX_MSG_MAP(CISEView)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISEView construction/destruction

CISEView::CISEView(): CExchangeView(CISEView::IDD)
{
	// TODO: add construction code here

}

CISEView::~CISEView()
{
}
void CISEView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CISEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CISEView drawing

void CISEView::OnDraw(CDC* pDC)
{
	CISEDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CISEView diagnostics

#ifdef _DEBUG
void CISEView::AssertValid() const
{
	CView::AssertValid();
}

void CISEView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CISEDoc* CISEView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CISEDoc)));
	return (CISEDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CISEView message handlers

void CISEView::OnDestroy() 
{
	CExchangeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CISEView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CExchangeView::OnTimer(nIDEvent);
}

void CISEView::OnSize(UINT nType, int cx, int cy) 
{
	CExchangeView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
