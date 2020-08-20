/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * LAVADoc.h : interface of the CLAVADoc class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *
 *******************************************************************************/

#if !defined(AFX_LAVADOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
#define AFX_LAVADOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLAVADoc : public CDocument
{
protected: // create from serialization only
	CLAVADoc();
	DECLARE_DYNCREATE(CLAVADoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLAVADoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLAVADoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLAVADoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAVADOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
