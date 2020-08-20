// ISEDoc.cpp : implementation of the CISEDoc class
//

#include "stdafx.h"
#include "ISE.h"

#include "ISEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISEDoc

IMPLEMENT_DYNCREATE(CISEDoc, CDocument)

BEGIN_MESSAGE_MAP(CISEDoc, CDocument)
	//{{AFX_MSG_MAP(CISEDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISEDoc construction/destruction

CISEDoc::CISEDoc()
{
	// TODO: add one-time construction code here

}

CISEDoc::~CISEDoc()
{
}

BOOL CISEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CISEDoc serialization

void CISEDoc::Serialize(CArchive& ar)
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
// CISEDoc diagnostics

#ifdef _DEBUG
void CISEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CISEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CISEDoc commands
