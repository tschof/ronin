/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * LAVAView.cpp : implementation of the CLAVAView class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *
 *******************************************************************************/

#include "stdafx.h"
#include "LAVA.h"

#include "LAVADoc.h"
#include "LAVAView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLAVAView

IMPLEMENT_DYNCREATE(CLAVAView, CFormView)

BEGIN_MESSAGE_MAP(CLAVAView, CFormView)
	//{{AFX_MSG_MAP(CLAVAView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLAVAView construction/destruction

CLAVAView::CLAVAView():CExchangeView(CLAVAView::IDD)
{
	// TODO: add construction code here

}

CLAVAView::~CLAVAView()
{
}

void CLAVAView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CLAVAView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLAVAView drawing

void CLAVAView::OnDraw(CDC* pDC)
{
	CLAVADoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CLAVAView diagnostics

#ifdef _DEBUG
void CLAVAView::AssertValid() const
{
	CView::AssertValid();
}

void CLAVAView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLAVADoc* CLAVAView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLAVADoc)));
	return (CLAVADoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLAVAView message handlers

void CLAVAView::OnDestroy()
{
	CExchangeView::OnDestroy();

	// TODO: Add your message handler code here

}

void CLAVAView::OnSize(UINT nType, int cx, int cy)
{
	CExchangeView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

}

void CLAVAView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CExchangeView::OnTimer(nIDEvent);
}
