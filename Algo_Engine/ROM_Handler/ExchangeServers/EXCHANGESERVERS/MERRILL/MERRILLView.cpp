/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLView.cpp : implementation of the CMERRILLView class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#include "stdafx.h"
#include "MERRILL.h"

#include "MERRILLDoc.h"
#include "MERRILLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMERRILLView

IMPLEMENT_DYNCREATE(CMERRILLView, CFormView)

BEGIN_MESSAGE_MAP(CMERRILLView, CFormView)
	//{{AFX_MSG_MAP(CMERRILLView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMERRILLView construction/destruction

CMERRILLView::CMERRILLView():CExchangeView(CMERRILLView::IDD)
{
	// TODO: add construction code here

}

CMERRILLView::~CMERRILLView()
{
}

void CMERRILLView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstinetView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}


BOOL CMERRILLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMERRILLView drawing

void CMERRILLView::OnDraw(CDC* pDC)
{
	CMERRILLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMERRILLView diagnostics

#ifdef _DEBUG
void CMERRILLView::AssertValid() const
{
	CView::AssertValid();
}

void CMERRILLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMERRILLDoc* CMERRILLView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMERRILLDoc)));
	return (CMERRILLDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMERRILLView message handlers

void CMERRILLView::OnDestroy()
{
	CExchangeView::OnDestroy();

	// TODO: Add your message handler code here

}

void CMERRILLView::OnSize(UINT nType, int cx, int cy)
{
	CExchangeView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

}

void CMERRILLView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CExchangeView::OnTimer(nIDEvent);
}
