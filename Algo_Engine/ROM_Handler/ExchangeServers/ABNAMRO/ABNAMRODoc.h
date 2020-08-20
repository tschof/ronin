// ABNAMRODoc.h : interface of the CABNAMRODoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABNAMRODOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_)
#define AFX_ABNAMRODOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CABNAMRODoc : public CDocument
{
protected: // create from serialization only
	CABNAMRODoc();
	DECLARE_DYNCREATE(CABNAMRODoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CABNAMRODoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CABNAMRODoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CABNAMRODoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABNAMRODOC_H__489E185E_6AC4_49B8_A160_7DEF2138462F__INCLUDED_)
