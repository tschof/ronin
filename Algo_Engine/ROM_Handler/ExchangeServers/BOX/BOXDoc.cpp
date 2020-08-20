// BOXDoc.cpp : implementation of the CBOXDoc class
//

#include "stdafx.h"
#include "BOX.h"

#include "BOXDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBOXDoc

IMPLEMENT_DYNCREATE(CBOXDoc, CDocument)

BEGIN_MESSAGE_MAP(CBOXDoc, CDocument)
	//{{AFX_MSG_MAP(CBOXDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBOXDoc construction/destruction

CBOXDoc::CBOXDoc()
{
	// TODO: add one-time construction code here

}

CBOXDoc::~CBOXDoc()
{
}

BOOL CBOXDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CBOXDoc serialization

void CBOXDoc::Serialize(CArchive& ar)
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
// CBOXDoc diagnostics

#ifdef _DEBUG
void CBOXDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBOXDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBOXDoc commands
