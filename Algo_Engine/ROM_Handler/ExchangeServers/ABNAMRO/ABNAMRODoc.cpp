// ABNAMRODoc.cpp : implementation of the CABNAMRODoc class
//

#include "stdafx.h"
#include "ABNAMRO.h"

#include "ABNAMRODoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CABNAMRODoc

IMPLEMENT_DYNCREATE(CABNAMRODoc, CDocument)

BEGIN_MESSAGE_MAP(CABNAMRODoc, CDocument)
	//{{AFX_MSG_MAP(CABNAMRODoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CABNAMRODoc construction/destruction

CABNAMRODoc::CABNAMRODoc()
{
	// TODO: add one-time construction code here

}

CABNAMRODoc::~CABNAMRODoc()
{
}

BOOL CABNAMRODoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CABNAMRODoc serialization

void CABNAMRODoc::Serialize(CArchive& ar)
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
// CABNAMRODoc diagnostics

#ifdef _DEBUG
void CABNAMRODoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CABNAMRODoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CABNAMRODoc commands
