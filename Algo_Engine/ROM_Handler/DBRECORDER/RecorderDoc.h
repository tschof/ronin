// RecorderDoc.h : interface of the CRecorderDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDERDOC_H__E2E8B3CE_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
#define AFX_RECORDERDOC_H__E2E8B3CE_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CRecorderDoc : public CDocument
{
protected: // create from serialization only
	CRecorderDoc();
	DECLARE_DYNCREATE(CRecorderDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecorderDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRecorderDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRecorderDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDERDOC_H__E2E8B3CE_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
