// ATDDoc.cpp : implementation of the CATDDoc class
//

#include "stdafx.h"
#include "ATD.h"

#include "ATDDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CATDDoc

IMPLEMENT_DYNCREATE(CATDDoc, CDocument)

BEGIN_MESSAGE_MAP(CATDDoc, CDocument)
	//{{AFX_MSG_MAP(CATDDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CATDDoc construction/destruction

CATDDoc::CATDDoc()
{
	// TODO: add one-time construction code here

}

CATDDoc::~CATDDoc()
{
}

BOOL CATDDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CATDDoc serialization

void CATDDoc::Serialize(CArchive& ar)
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
// CATDDoc diagnostics

#ifdef _DEBUG
void CATDDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CATDDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CATDDoc commands
