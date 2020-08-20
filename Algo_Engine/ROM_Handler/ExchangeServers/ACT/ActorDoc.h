// ActorDoc.h : interface of the CActorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTORDOC_H__FECD1FEC_5328_11D2_A84B_006008CD4894__INCLUDED_)
#define AFX_ACTORDOC_H__FECD1FEC_5328_11D2_A84B_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CActorDoc : public CDocument
{
protected: // create from serialization only
	CActorDoc();
	DECLARE_DYNCREATE(CActorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CActorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CActorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTORDOC_H__FECD1FEC_5328_11D2_A84B_006008CD4894__INCLUDED_)
