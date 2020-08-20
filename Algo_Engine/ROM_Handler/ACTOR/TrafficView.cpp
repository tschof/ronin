// TrafficView.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TrafficView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrafficView

IMPLEMENT_DYNCREATE(CTrafficView, CFormView)

CTrafficView::CTrafficView()
	: CFormView(CTrafficView::IDD)
{
	//{{AFX_DATA_INIT(CTrafficView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTrafficView::~CTrafficView()
{
}

void CTrafficView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrafficView)
	DDX_Control(pDX, IDC_TRAFFIC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrafficView, CFormView)
	//{{AFX_MSG_MAP(CTrafficView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficView diagnostics

#ifdef _DEBUG
void CTrafficView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTrafficView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTrafficView message handlers

void CTrafficView::Add(const CString& Message)
{
	if (m_List.GetCount() > 200)
		m_List.DeleteString(0);

	CString	Tmp;
	int		Index;

	Tmp = CTime::GetCurrentTime().Format("%H%M%S") + '\t' + Message;
	Index = m_List.AddString(Tmp);
	m_List.SetCurSel(Index);
}

void CTrafficView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect		ListRect;
	
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

void CTrafficView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	// force a resize
		CRect Rect;
		GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		OnSize(0, Rect.right, Rect.bottom);
}
