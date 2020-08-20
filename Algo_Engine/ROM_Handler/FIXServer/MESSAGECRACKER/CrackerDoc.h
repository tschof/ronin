// CrackerDoc.h : interface of the CCrackerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRACKERDOC_H__9C319C34_716D_4580_86E1_605E6D3BAD11__INCLUDED_)
#define AFX_CRACKERDOC_H__9C319C34_716D_4580_86E1_605E6D3BAD11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CCrackerApp;
class CCrackerDoc : public CDocument
{
protected: // create from serialization only
	CCrackerDoc();
	DECLARE_DYNCREATE(CCrackerDoc)

// Attributes
public:
	CCrackerApp *				m_pApp;
	tMessages					m_Messages;
	CString						m_szPathName;

// Operations
public:
	void LoadLogFile	();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCrackerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCrackerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKERDOC_H__9C319C34_716D_4580_86E1_605E6D3BAD11__INCLUDED_)
