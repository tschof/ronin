// CBOEDoc.cpp : implementation of the CCBOEDoc class
//

#include "stdafx.h"
#include "CBOE.h"

#include "CBOEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCBOEDoc

IMPLEMENT_DYNCREATE(CCBOEDoc, CDocument)

BEGIN_MESSAGE_MAP(CCBOEDoc, CDocument)
	//{{AFX_MSG_MAP(CCBOEDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCBOEDoc construction/destruction

CCBOEDoc::CCBOEDoc()
{
	// TODO: add one-time construction code here

}

CCBOEDoc::~CCBOEDoc()
{
}

BOOL CCBOEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCBOEDoc serialization

void CCBOEDoc::Serialize(CArchive& ar)
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
// CCBOEDoc diagnostics

#ifdef _DEBUG
void CCBOEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCBOEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCBOEDoc commands
