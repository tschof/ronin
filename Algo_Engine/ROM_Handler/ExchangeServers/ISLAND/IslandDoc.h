// IslandDoc.h : interface of the CIslandDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDDOC_H__AF6E8EAE_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDDOC_H__AF6E8EAE_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CIslandDoc : public CDocument
{
protected: // create from serialization only
	CIslandDoc();
	DECLARE_DYNCREATE(CIslandDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIslandDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIslandDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIslandDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISLANDDOC_H__AF6E8EAE_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
