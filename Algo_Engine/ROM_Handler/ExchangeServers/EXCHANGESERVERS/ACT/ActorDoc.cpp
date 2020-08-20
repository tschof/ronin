// ActorDoc.cpp : implementation of the CActorDoc class
//

#include "stdafx.h"
#include "Actor.h"

#include "ActorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActorDoc

IMPLEMENT_DYNCREATE(CActorDoc, CDocument)

BEGIN_MESSAGE_MAP(CActorDoc, CDocument)
	//{{AFX_MSG_MAP(CActorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActorDoc construction/destruction

CActorDoc::CActorDoc()
{
	// TODO: add one-time construction code here

}

CActorDoc::~CActorDoc()
{
}

BOOL CActorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CActorDoc serialization

void CActorDoc::Serialize(CArchive& ar)
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
// CActorDoc diagnostics

#ifdef _DEBUG
void CActorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CActorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CActorDoc commands
