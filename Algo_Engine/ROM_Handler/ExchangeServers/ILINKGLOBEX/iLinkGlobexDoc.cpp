//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"

#include "iLinkGlobexDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CILinkGlobexDoc, CDocument)

BEGIN_MESSAGE_MAP(CILinkGlobexDoc, CDocument)
	//{{AFX_MSG_MAP(CILinkGlobexDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexDoc construction/destruction

CILinkGlobexDoc::CILinkGlobexDoc()
{
	// TODO: add one-time construction code here

}

CILinkGlobexDoc::~CILinkGlobexDoc()
{
}

BOOL CILinkGlobexDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexDoc serialization

void CILinkGlobexDoc::Serialize(CArchive& ar)
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
// CILinkGlobexDoc diagnostics

#ifdef _DEBUG
void CILinkGlobexDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CILinkGlobexDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CILinkGlobexDoc commands
