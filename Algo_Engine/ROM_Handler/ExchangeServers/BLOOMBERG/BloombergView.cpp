// BloombergView.cpp : implementation of the CBloombergView class
//

#include "stdafx.h"
#include "Bloomberg.h"

#include "BloombergDoc.h"
#include "BloombergView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBloombergView

IMPLEMENT_DYNCREATE(CBloombergView, CFormView)

BEGIN_MESSAGE_MAP(CBloombergView, CFormView)
	//{{AFX_MSG_MAP(CBloombergView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()

	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBloombergView construction/destruction

CBloombergView::CBloombergView()
	: CFormView(CBloombergView::IDD)
{
	//{{AFX_DATA_INIT(CBloombergView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_TimerId = 0;
	m_TimerConnectionCheckID = 0;
}

CBloombergView::~CBloombergView()
{
}

void CBloombergView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBloombergView)
	DDX_Control(pDX, IDC_CONNECTION_LIST, m_List);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CBloombergView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}



void CBloombergView::OnClose() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}
	
	if (m_TimerConnectionCheckID >= 0)
	{
		KillTimer(m_TimerConnectionCheckID);
		m_TimerConnectionCheckID = -1;
	}

	CFormView::OnClose();
}

void CBloombergView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	if (m_TimerConnectionCheckID >= 0)
	{
		KillTimer(m_TimerConnectionCheckID);
		m_TimerConnectionCheckID = -1;
	}

	CFormView::OnDestroy();
}

void CBloombergView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_TimerId)
	{
		if (m_TimerId >= 0)
		{
			CMessageApp *pApp = (CMessageApp *)AfxGetApp();
			pApp->Exchange().ProcessTimer();
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CBloombergView::OnSize(UINT nType, int cx, int cy) 
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

void CBloombergView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}


void CBloombergView::CreateConnectionCheckTimer(UINT Interval)
{
	if (m_TimerConnectionCheckID >= 0)
	{
		KillTimer(m_TimerConnectionCheckID);
	}

	m_TimerConnectionCheckID = SetTimer(2, Interval, NULL);
}

void CBloombergView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// force a resize
	CRect Rect;
	GetWindowRect(&Rect);
	ScreenToClient(&Rect);
	OnSize(0, Rect.right, Rect.bottom);	
}

/////////////////////////////////////////////////////////////////////////////
// CBloombergView diagnostics

#ifdef _DEBUG
void CBloombergView::AssertValid() const
{
	CFormView::AssertValid();
}

void CBloombergView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CBloombergDoc* CBloombergView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBloombergDoc)));
	return (CBloombergDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CBloombergView message handlers
