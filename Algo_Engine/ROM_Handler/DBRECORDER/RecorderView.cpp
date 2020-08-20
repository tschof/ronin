// RecorderView.cpp : implementation of the CRecorderView class
//

#include "stdafx.h"
#include "DBRecorder.h"

#include "RecorderDoc.h"
#include "RecorderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecorderView

IMPLEMENT_DYNCREATE(CRecorderView, CFormView)

BEGIN_MESSAGE_MAP(CRecorderView, CFormView)
	//{{AFX_MSG_MAP(CRecorderView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderView construction/destruction

CRecorderView::CRecorderView()
	: CFormView(CRecorderView::IDD)
{
	//{{AFX_DATA_INIT(CRecorderView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_Timer = -1;

}

CRecorderView::~CRecorderView()
{
}

void CRecorderView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecorderView)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}

BOOL CRecorderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView diagnostics

#ifdef _DEBUG
void CRecorderView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRecorderView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CRecorderDoc* CRecorderView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRecorderDoc)));
	return (CRecorderDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRecorderView message handlers

void CRecorderView::OnSize(UINT nType, int cx, int cy) 
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

void CRecorderView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_Timer = SetTimer(1, 10000, NULL);

	// force a resize
		CRect Rect;
		GetWindowRect(&Rect);
		ScreenToClient(&Rect);
		OnSize(0, Rect.right, Rect.bottom);
}

void CRecorderView::OnTimer(UINT nIDEvent) 
{
	CRecorderApp	*pApp			= (CRecorderApp *)AfxGetApp();
	CTime				CurrentTime	= CTime::GetCurrentTime();
	
	m_List.Update();


	int StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",	6,		pApp->DatFile());
	int EndTime = GetPrivateProfileInt("Runtime Information",		"End Time",		23,	pApp->DatFile());

	if(StartTime == EndTime)
	{
		if((CurrentTime.GetHour() == StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
		}
	}
	else if(StartTime > EndTime)
	{
		if ((CurrentTime.GetHour() >= EndTime) && (CurrentTime.GetHour() < StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
		}
	}
	else
	{
		if ((CurrentTime.GetHour() < StartTime) || (CurrentTime.GetHour() >= EndTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CRecorderView::OnDestroy() 
{
	if (m_Timer >= 0)
		KillTimer(m_Timer);

	m_Timer = -1;

	CFormView::OnDestroy();
}

void CRecorderView::OnClose() 
{
	if (m_Timer >= 0)
		KillTimer(m_Timer);

	m_Timer = -1;
	
	CFormView::OnClose();
}
