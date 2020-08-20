// InstinetDoc.h : interface of the CInstinetDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETDOC_H__E081795E_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINETDOC_H__E081795E_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CInstinetDoc : public CDocument
{
protected: // create from serialization only
	CInstinetDoc();
	DECLARE_DYNCREATE(CInstinetDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstinetDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstinetDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInstinetDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTINETDOC_H__E081795E_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
