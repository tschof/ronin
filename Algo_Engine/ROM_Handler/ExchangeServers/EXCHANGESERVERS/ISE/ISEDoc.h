// ISEDoc.h : interface of the CISEDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISEDOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_)
#define AFX_ISEDOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CISEDoc : public CDocument
{
protected: // create from serialization only
	CISEDoc();
	DECLARE_DYNCREATE(CISEDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISEDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CISEDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CISEDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISEDOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_)
