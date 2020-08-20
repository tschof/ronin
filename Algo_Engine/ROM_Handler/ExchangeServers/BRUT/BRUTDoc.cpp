// BRUTDoc.cpp : implementation of the CBRUTDoc class
//

#include "stdafx.h"
#include "BRUT.h"
#include "BRUTDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBRUTDoc

IMPLEMENT_DYNCREATE(CBRUTDoc, CDocument)

BEGIN_MESSAGE_MAP(CBRUTDoc, CDocument)
	//{{AFX_MSG_MAP(CBRUTDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBRUTDoc construction/destruction

CBRUTDoc::CBRUTDoc()
{
	// TODO: add one-time construction code here

}

CBRUTDoc::~CBRUTDoc()
{
}

BOOL CBRUTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CBRUTDoc serialization

void CBRUTDoc::Serialize(CArchive& ar)
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
// CBRUTDoc diagnostics

#ifdef _DEBUG
void CBRUTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBRUTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBRUTDoc commands
