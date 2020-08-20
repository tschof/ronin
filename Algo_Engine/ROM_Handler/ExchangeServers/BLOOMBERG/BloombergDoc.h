// BloombergDoc.h : interface of the CBloombergDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOOMBERGDOC_H__F46917BD_DFCC_11D3_A600_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERGDOC_H__F46917BD_DFCC_11D3_A600_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBloombergDoc : public CDocument
{
protected: // create from serialization only
	CBloombergDoc();
	DECLARE_DYNCREATE(CBloombergDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBloombergDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBloombergDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBloombergDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOOMBERGDOC_H__F46917BD_DFCC_11D3_A600_00500401F87A__INCLUDED_)
