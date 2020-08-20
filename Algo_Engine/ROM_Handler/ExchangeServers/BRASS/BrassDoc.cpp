// BrassDoc.cpp : implementation of the CBrassDoc class
//

#include "stdafx.h"
#include "Brass.h"
#include "BrassDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrassDoc

IMPLEMENT_DYNCREATE(CBrassDoc, CDocument)

BEGIN_MESSAGE_MAP(CBrassDoc, CDocument)
	//{{AFX_MSG_MAP(CBrassDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrassDoc construction/destruction

CBrassDoc::CBrassDoc()
{
	// TODO: add one-time construction code here

}

CBrassDoc::~CBrassDoc()
{
}

BOOL CBrassDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CBrassDoc serialization

void CBrassDoc::Serialize(CArchive& ar)
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
// CBrassDoc diagnostics

#ifdef _DEBUG
void CBrassDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBrassDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrassDoc commands
