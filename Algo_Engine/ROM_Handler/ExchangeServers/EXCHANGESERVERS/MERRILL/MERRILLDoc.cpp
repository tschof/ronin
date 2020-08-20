/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLDoc.cpp : implementation of the CMERRILLDoc class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#include "stdafx.h"
#include "MERRILL.h"

#include "MERRILLDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMERRILLDoc

IMPLEMENT_DYNCREATE(CMERRILLDoc, CDocument)

BEGIN_MESSAGE_MAP(CMERRILLDoc, CDocument)
	//{{AFX_MSG_MAP(CMERRILLDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMERRILLDoc construction/destruction

CMERRILLDoc::CMERRILLDoc()
{
	// TODO: add one-time construction code here

}

CMERRILLDoc::~CMERRILLDoc()
{
}

BOOL CMERRILLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMERRILLDoc serialization

void CMERRILLDoc::Serialize(CArchive& ar)
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
// CMERRILLDoc diagnostics

#ifdef _DEBUG
void CMERRILLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMERRILLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMERRILLDoc commands
