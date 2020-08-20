// TrafficDoc.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrafficDoc

IMPLEMENT_DYNCREATE(CTrafficDoc, CDocument)

CTrafficDoc::CTrafficDoc()
{
}

BOOL CTrafficDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTrafficDoc::~CTrafficDoc()
{
}


BEGIN_MESSAGE_MAP(CTrafficDoc, CDocument)
	//{{AFX_MSG_MAP(CTrafficDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficDoc diagnostics

#ifdef _DEBUG
void CTrafficDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTrafficDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTrafficDoc serialization

void CTrafficDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTrafficDoc commands

BOOL CTrafficDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
return FALSE;
}
