// ATDDoc.h : interface of the CATDDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATDDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
#define AFX_ATDDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CATDDoc : public CDocument
{
protected: // create from serialization only
	CATDDoc();
	DECLARE_DYNCREATE(CATDDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATDDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CATDDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CATDDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATDDOC_H__47DDFC7A_63B3_41F6_9DE6_A6520C37E9BE__INCLUDED_)
