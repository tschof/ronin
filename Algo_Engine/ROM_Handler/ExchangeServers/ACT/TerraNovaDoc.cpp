// TerraNovaDoc.cpp : implementation of the CTerraNovaDoc class
//

#include "stdafx.h"
#include "Actor.h"

#include "TerraNovaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaDoc

IMPLEMENT_DYNCREATE(CTerraNovaDoc, CDocument)

BEGIN_MESSAGE_MAP(CTerraNovaDoc, CDocument)
	//{{AFX_MSG_MAP(CTerraNovaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaDoc construction/destruction

CTerraNovaDoc::CTerraNovaDoc()
{
	// TODO: add one-time construction code here

}

CTerraNovaDoc::~CTerraNovaDoc()
{
}

BOOL CTerraNovaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTerraNovaDoc serialization

void CTerraNovaDoc::Serialize(CArchive& ar)
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
// CTerraNovaDoc diagnostics

#ifdef _DEBUG
void CTerraNovaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTerraNovaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaDoc commands
