/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * LAVADoc.cpp : implementation of the CLAVADoc class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *
 *******************************************************************************/

#include "stdafx.h"
#include "LAVA.h"

#include "LAVADoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLAVADoc

IMPLEMENT_DYNCREATE(CLAVADoc, CDocument)

BEGIN_MESSAGE_MAP(CLAVADoc, CDocument)
	//{{AFX_MSG_MAP(CLAVADoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLAVADoc construction/destruction

CLAVADoc::CLAVADoc()
{
	// TODO: add one-time construction code here

}

CLAVADoc::~CLAVADoc()
{
}

BOOL CLAVADoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLAVADoc serialization

void CLAVADoc::Serialize(CArchive& ar)
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
// CLAVADoc diagnostics

#ifdef _DEBUG
void CLAVADoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLAVADoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLAVADoc commands
