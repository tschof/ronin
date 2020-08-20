// IslandDoc.cpp : implementation of the CIslandDoc class
//

#include "stdafx.h"
#include "Island.h"

#include "IslandDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIslandDoc

IMPLEMENT_DYNCREATE(CIslandDoc, CDocument)

BEGIN_MESSAGE_MAP(CIslandDoc, CDocument)
	//{{AFX_MSG_MAP(CIslandDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIslandDoc construction/destruction

CIslandDoc::CIslandDoc()
{
	// TODO: add one-time construction code here

}

CIslandDoc::~CIslandDoc()
{
}

BOOL CIslandDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CIslandDoc serialization

void CIslandDoc::Serialize(CArchive& ar)
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
// CIslandDoc diagnostics

#ifdef _DEBUG
void CIslandDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIslandDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIslandDoc commands
