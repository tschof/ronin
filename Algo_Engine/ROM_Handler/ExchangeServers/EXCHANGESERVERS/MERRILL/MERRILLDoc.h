/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLDoc.h : interface of the CMERRILLDoc class
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#if !defined(AFX_MERRILLDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
#define AFX_MERRILLDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMERRILLDoc : public CDocument
{
protected: // create from serialization only
	CMERRILLDoc();
	DECLARE_DYNCREATE(CMERRILLDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMERRILLDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMERRILLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMERRILLDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERRILLDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
