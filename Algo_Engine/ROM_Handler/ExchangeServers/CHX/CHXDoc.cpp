// CHXDoc.cpp : implementation of the CCHXDoc class
//

#include "stdafx.h"
#include "CHX.h"
#include "CHXDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCHXDoc

IMPLEMENT_DYNCREATE(CCHXDoc, CDocument)

BEGIN_MESSAGE_MAP(CCHXDoc, CDocument)
	//{{AFX_MSG_MAP(CCHXDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCHXDoc construction/destruction

CCHXDoc::CCHXDoc()
{
	// TODO: add one-time construction code here

}

CCHXDoc::~CCHXDoc()
{
}

BOOL CCHXDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCHXDoc serialization

void CCHXDoc::Serialize(CArchive& ar)
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
// CCHXDoc diagnostics

#ifdef _DEBUG
void CCHXDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCHXDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCHXDoc commands
