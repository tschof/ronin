// OMFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectionFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COMFrame

IMPLEMENT_DYNCREATE(CConnectionFrame, CMDIChildWnd)

CConnectionFrame::CConnectionFrame() :
	CChildFrame()
{
}

CConnectionFrame::~CConnectionFrame()
{
}


BEGIN_MESSAGE_MAP(CConnectionFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CConnectionFrame)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionFrame message handlers

void CConnectionFrame::OnDestroy() 
{
	m_Placement.Save();
	CMDIChildWnd::OnDestroy();
}

void CConnectionFrame::OnClose() 
{
	m_Placement.Save();
	CMDIChildWnd::OnClose();
}
