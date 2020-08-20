// TerraNovaDoc.cpp : implementation of the CArcaDoc class
//

#include "stdafx.h"
#include "TerraNova.h"
#include "TerraNovaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArcaDoc

IMPLEMENT_DYNCREATE(CArcaDoc, CDocument)

BEGIN_MESSAGE_MAP(CArcaDoc, CDocument)
	//{{AFX_MSG_MAP(CArcaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArcaDoc construction/destruction

CArcaDoc::CArcaDoc()
{
	// TODO: add one-time construction code here

}

CArcaDoc::~CArcaDoc()
{
}

BOOL CArcaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CArcaDoc serialization

void CArcaDoc::Serialize(CArchive& ar)
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
// CArcaDoc diagnostics

#ifdef _DEBUG
void CArcaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CArcaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CArcaDoc commands
