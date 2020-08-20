// ArcaDirectDoc.cpp : implementation of the CArcaDirectDoc class
//

#include "stdafx.h"
#include "ArcaDirect.h"
#include "ArcaDirectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectDoc

IMPLEMENT_DYNCREATE(CArcaDirectDoc, CDocument)

BEGIN_MESSAGE_MAP(CArcaDirectDoc, CDocument)
	//{{AFX_MSG_MAP(CArcaDirectDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectDoc construction/destruction

CArcaDirectDoc::CArcaDirectDoc()
{
	// TODO: add one-time construction code here

}

CArcaDirectDoc::~CArcaDirectDoc()
{
}

BOOL CArcaDirectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CArcaDirectDoc serialization

void CArcaDirectDoc::Serialize(CArchive& ar)
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
// CArcaDirectDoc diagnostics

#ifdef _DEBUG
void CArcaDirectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CArcaDirectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectDoc commands
