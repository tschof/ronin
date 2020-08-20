// OneChicagoView.cpp : implementation of the COneChicagoView class
//

#include "stdafx.h"
#include "OneChicago.h"

#include "OneChicagoDoc.h"
#include "OneChicagoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COneChicagoView

IMPLEMENT_DYNCREATE(COneChicagoView, CFormView)

BEGIN_MESSAGE_MAP(COneChicagoView, CFormView)
	//{{AFX_MSG_MAP(COneChicagoView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COneChicagoView construction/destruction

COneChicagoView::COneChicagoView():CExchangeView(COneChicagoView::IDD)
{
	// TODO: add construction code here

}

COneChicagoView::~COneChicagoView()
{
}

void COneChicagoView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL COneChicagoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COneChicagoView drawing

void COneChicagoView::OnDraw(CDC* pDC)
{
	COneChicagoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// COneChicagoView diagnostics

#ifdef _DEBUG
void COneChicagoView::AssertValid() const
{
	CView::AssertValid();
}

void COneChicagoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COneChicagoDoc* COneChicagoView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COneChicagoDoc)));
	return (COneChicagoDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COneChicagoView message handlers

void COneChicagoView::OnDestroy() 
{
	CExchangeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void COneChicagoView::OnSize(UINT nType, int cx, int cy) 
{
	CExchangeView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void COneChicagoView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CExchangeView::OnTimer(nIDEvent);
}
