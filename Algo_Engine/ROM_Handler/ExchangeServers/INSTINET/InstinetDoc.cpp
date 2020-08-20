// InstinetDoc.cpp : implementation of the CInstinetDoc class
//

#include "stdafx.h"
#include "Instinet.h"

#include "InstinetDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstinetDoc

IMPLEMENT_DYNCREATE(CInstinetDoc, CDocument)

BEGIN_MESSAGE_MAP(CInstinetDoc, CDocument)
	//{{AFX_MSG_MAP(CInstinetDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstinetDoc construction/destruction

CInstinetDoc::CInstinetDoc()
{
	// TODO: add one-time construction code here

}

CInstinetDoc::~CInstinetDoc()
{
}

BOOL CInstinetDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CInstinetDoc serialization

void CInstinetDoc::Serialize(CArchive& ar)
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
// CInstinetDoc diagnostics

#ifdef _DEBUG
void CInstinetDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CInstinetDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInstinetDoc commands
