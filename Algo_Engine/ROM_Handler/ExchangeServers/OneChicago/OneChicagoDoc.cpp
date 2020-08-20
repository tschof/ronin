// OneChicagoDoc.cpp : implementation of the COneChicagoDoc class
//

#include "stdafx.h"
#include "OneChicago.h"

#include "OneChicagoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COneChicagoDoc

IMPLEMENT_DYNCREATE(COneChicagoDoc, CDocument)

BEGIN_MESSAGE_MAP(COneChicagoDoc, CDocument)
	//{{AFX_MSG_MAP(COneChicagoDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COneChicagoDoc construction/destruction

COneChicagoDoc::COneChicagoDoc()
{
	// TODO: add one-time construction code here

}

COneChicagoDoc::~COneChicagoDoc()
{
}

BOOL COneChicagoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// COneChicagoDoc serialization

void COneChicagoDoc::Serialize(CArchive& ar)
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
// COneChicagoDoc diagnostics

#ifdef _DEBUG
void COneChicagoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COneChicagoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COneChicagoDoc commands
